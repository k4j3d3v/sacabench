# CaPS-SA Integration Report

## Overview
This document details the complete process of integrating the CaPS-SA (Cache-aware Parallel Suffix Array) algorithm into the sacabench project. The integration involved resolving build system issues, template compilation problems, and ensuring proper CMake configuration propagation.

## Initial Problem
The CaPS-SA algorithm was present in the codebase but was disabled and non-functional due to:
- Commented-out build configuration
- Missing header files and type definitions
- Template instantiation/linking errors
- Improper SIMD detection and configuration
- Hardcoded compile-time settings instead of CMake-managed ones

## Debugging Process & Solutions

### 1. Build System Re-enablement

**Problem**: CaPS-SA was completely disabled in the build system.

**Root Cause**: The algorithm subdirectory and linking were commented out in CMakeLists.txt files.

**Solution**: 
- Re-enabled CaPS-SA in `/CMakeLists.txt` by uncommenting:
  ```cmake
  add_subdirectory(external/reference_impls/CaPS-SA)
  ```
- Re-enabled linking in `/sacabench/CMakeLists.txt` by uncommenting:
  ```cmake
  target_link_libraries(sacabench_saca PRIVATE caps_sa)
  ```
- Re-registered the algorithm in `/sacabench/saca/register.cmake` by uncommenting:
  ```cmake
  register_saca_algorithm(caps-sa)
  ```

**CMake Explanation**: 
- `add_subdirectory()` tells CMake to process another CMakeLists.txt file in a subdirectory
- `target_link_libraries()` creates a dependency between two targets, ensuring one links against another
- The custom `register_saca_algorithm()` macro makes the algorithm available to the benchmark framework

### 2. Missing Type Definitions

**Problem**: Compilation failed due to undefined types and constants.

**Diagnostic Commands Used**:
```bash
make 2>&1 | grep -A5 -B5 "error:"
```

**Root Cause**: CaPS-SA code referenced types that weren't defined in the utility headers.

**Solution**: Added missing definitions to `/external/reference_impls/CaPS-SA/include/utility.hpp`:
```cpp
constexpr std::size_t L1_CACHE_LINE_SIZE = 64; // Fallback, overridden by CMake

template<typename T>
struct Padded {
    alignas(L1_CACHE_LINE_SIZE) T value;
    Padded() = default;
    Padded(const T& v) : value(v) {}
    operator T&() { return value; }
    operator const T&() const { return value; }
};

template<typename T>
using Buffer = std::vector<T>;
```

**CMake Explanation**: The `constexpr` provides a compile-time fallback value, but CMake will override this with `add_compile_definitions(L1_CACHE_LINE_SIZE=${detected_value})`.

### 3. SIMD Header Detection Issues

**Problem**: SIMD headers weren't being included correctly, causing compilation failures.

**Root Cause**: The SIMD detection logic in `Genomic_Text.hpp` wasn't properly coordinated with CMake settings.

**Initial Workaround**: Temporarily forced native SIMD:
```cpp
#define NATIVE_SIMD
#include <immintrin.h>
```

**Proper Solution**: Fixed CMake to properly detect and propagate SIMD capabilities:
- CMake detects x86_64 architecture and sets `NATIVE_SIMD=TRUE`
- This gets propagated via `add_compile_definitions(NATIVE_SIMD)`
- The header uses `#ifdef NATIVE_SIMD` to conditionally include the right headers

**CMake Explanation**: 
- `add_compile_definitions()` adds preprocessor definitions (like `#define`) to all source files
- `#ifdef` in C++ checks if a preprocessor symbol is defined
- This allows the same source code to work on different architectures

### 4. Template Instantiation/Linking Errors

**Problem**: Linker couldn't find template instantiations for required type combinations.

**Diagnostic Process**: 
```bash
make 2>&1 | grep "undefined reference"
```

**Root Cause**: C++ templates are only compiled when used, but CaPS-SA was built as a separate library. The linker couldn't find concrete implementations for the template combinations that sacabench needed.

**Solution**: Added explicit template instantiations to `/external/reference_impls/CaPS-SA/src/Suffix_Array.cpp`:
```cpp
// Explicit template instantiations for all types sacabench might use
template void construct_SA<uint32_t, uint8_t>(/* ... */);
template void construct_SA<uint32_t, uint32_t>(/* ... */);
template void construct_SA<uint64_t, uint8_t>(/* ... */);
template void construct_SA<uint64_t, uint32_t>(/* ... */);
template void construct_SA<int32_t, uint8_t>(/* ... */);
template void construct_SA<int32_t, uint32_t>(/* ... */);
template void construct_SA<int64_t, uint8_t>(/* ... */);
template void construct_SA<int64_t, uint32_t>(/* ... */);
```

**CMake Explanation**: When building static libraries with templates, you need explicit instantiations so the compiler generates the actual machine code for specific type combinations.

### 5. CMake Configuration Propagation Issues

**Problem**: Compile definitions weren't being propagated to consuming libraries.

**Root Cause**: The `target_compile_definitions()` calls were placed before the library target was created.

**Solution**: Moved compile definitions after library creation in `/external/reference_impls/CaPS-SA/CMakeLists.txt`:
```cmake
add_library(caps_sa)
# ... other configuration ...

# MOVED THESE AFTER add_library():
target_compile_definitions(caps_sa PUBLIC NATIVE_SIMD)
target_compile_definitions(caps_sa PUBLIC L1_CACHE_LINE_SIZE=${L1_CACHE_LINE_SIZE})
```

**CMake Explanation**: 
- `target_compile_definitions(target PUBLIC ...)` makes definitions available to both the target itself and anything that links to it
- `PUBLIC` means the definition is part of the target's interface
- You can only call `target_*` functions after the target exists (after `add_library()`)

### 6. Cache Line Size Detection

**Problem**: The build system needed to detect the system's cache line size at build time.

**Solution**: Added platform-specific detection in CMakeLists.txt:
```cmake
# Cache line size detection
if(OS_NAME STREQUAL "Linux")
    execute_process(
        COMMAND getconf LEVEL1_DCACHE_LINESIZE
        OUTPUT_VARIABLE L1_CACHE_LINE_SIZE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
elseif(OS_NAME STREQUAL "macOS")
    if(ARCH STREQUAL "x86_64")
        execute_process(
            COMMAND sysctl machdep.cpu.cache.linesize
            COMMAND awk "{print $2}"
            OUTPUT_VARIABLE L1_CACHE_LINE_SIZE
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    elseif(ARCH STREQUAL "AppleSilicon")
        execute_process(
            COMMAND sysctl hw.cachelinesize
            COMMAND awk "{print $2}"
            OUTPUT_VARIABLE L1_CACHE_LINE_SIZE
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    endif()
endif()

add_compile_definitions(L1_CACHE_LINE_SIZE=${L1_CACHE_LINE_SIZE})
```

**CMake Explanation**:
- `execute_process()` runs shell commands at build configuration time
- `OUTPUT_VARIABLE` captures the command output into a CMake variable
- `OUTPUT_STRIP_TRAILING_WHITESPACE` removes newlines from the output
- The detected value is then used in `add_compile_definitions()` to make it available to C++ code

### 7. Wrapper Interface Updates

**Problem**: The sacabench wrapper wasn't using the correct template types.

**Solution**: Updated `/sacabench/saca/external/caps-sa.hpp` to use proper template instantiation:
```cpp
template <typename AlphabetType, typename IndexType>
void construct_sa(AlphabetType const* text, IndexType const n, IndexType* sa) {
    // Use the properly typed interface without casts
    CaPS_SA::construct_SA<IndexType, AlphabetType>(sa, text, static_cast<std::size_t>(n));
}
```

**CMake Explanation**: Templates allow the same code to work with different types, but the compiler needs to know which specific type combinations to generate code for.

## Key CMake Concepts Learned

### 1. Target-Based Build System
Modern CMake uses "targets" (like `caps_sa`) rather than global variables. Each target has:
- **Sources**: What files to compile
- **Include Directories**: Where to find headers
- **Compile Definitions**: Preprocessor macros
- **Link Libraries**: What other targets/libraries to link against
- **Compile Options**: Compiler flags

### 2. Visibility Keywords
- **PUBLIC**: Available to this target AND anything that links to it
- **PRIVATE**: Only available to this target
- **INTERFACE**: Only available to things that link to this target

### 3. Configuration vs Build Time
- **Configuration Time**: When you run `cmake` - variables are set, files are generated
- **Build Time**: When you run `make` - source files are compiled and linked

### 4. External Dependencies
The project uses `ExternalProject_Add()` to automatically download and build dependencies like `parlaylib` and `SIMDe`.

## Final Verification

After all fixes, the project builds successfully with:
```bash
make
```

The integration is complete and CaPS-SA is now available as one of the suffix array algorithms in sacabench.

## Files Modified

1. **`/CMakeLists.txt`** - Re-enabled CaPS-SA subdirectory
2. **`/sacabench/CMakeLists.txt`** - Re-enabled CaPS-SA linking  
3. **`/sacabench/saca/register.cmake`** - Re-registered CaPS-SA algorithm
4. **`/sacabench/saca/external/caps-sa.hpp`** - Fixed wrapper template usage
5. **`/external/reference_impls/CaPS-SA/CMakeLists.txt`** - Fixed configuration propagation
6. **`/external/reference_impls/CaPS-SA/include/utility.hpp`** - Added missing types
7. **`/external/reference_impls/CaPS-SA/include/Genomic_Text.hpp`** - Fixed SIMD headers
8. **`/external/reference_impls/CaPS-SA/src/Suffix_Array.cpp`** - Added explicit template instantiations

## Lessons Learned

1. **Template Libraries**: When building template code as separate libraries, explicit instantiation is often required
2. **CMake Dependencies**: Target properties must be set AFTER target creation
3. **Cross-Platform**: Different systems require different commands for hardware detection
4. **Build System Integration**: Large projects need careful coordination between multiple CMake files
5. **SIMD Portability**: Conditional compilation allows the same code to work with and without SIMD

The integration demonstrates how modern C++ projects manage complex dependencies, cross-platform compatibility, and performance optimizations through sophisticated build systems.
