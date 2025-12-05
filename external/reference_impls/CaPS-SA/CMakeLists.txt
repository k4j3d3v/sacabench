cmake_minimum_required(VERSION 3.14)

# ================================================================
#  Project
# ================================================================
project(caps_sa VERSION 0.1.0 LANGUAGES CXX)

set(CAPS_SA_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# ================================================================
#  Standard / Toolchain
# ================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(ASAN OR UBSAN)
    set(CLANG TRUE)
endif()

if(CLANG)
    set(CMAKE_C_COMPILER clang)
    set(CMAKE_CXX_COMPILER clang++)
endif()

# ================================================================
#  Flags
# ================================================================
set(WARNING_FLAGS -Wall -Wextra -Wpedantic)
set(SUPPRESS_WARNING_FLAGS -Wno-self-assign-overloaded)
set(DEBUG_FLAGS -ggdb)
set(ASAN_FLAGS -fsanitize=address -fno-omit-frame-pointer)
set(UBSAN_FLAGS -fsanitize=undefined
                -fsanitize=float-divide-by-zero
                -fsanitize=implicit-conversion
                -fsanitize=local-bounds
                -fsanitize=nullability)
set(OPTIMIZE_FLAGS -Ofast -funroll-loops -march=native -mavx2 -DNATIVE_SIMD)

set(COMPILE_FLAGS "")
set(LINK_FLAGS "")

list(APPEND COMPILE_FLAGS ${WARNING_FLAGS} ${SUPPRESS_WARNING_FLAGS} ${OPTIMIZE_FLAGS})

if(ASAN)
    list(APPEND COMPILE_FLAGS ${ASAN_FLAGS} ${DEBUG_FLAGS})
    list(APPEND LINK_FLAGS ${ASAN_FLAGS})
endif()

if(UBSAN)
    list(APPEND COMPILE_FLAGS ${UBSAN_FLAGS} ${DEBUG_FLAGS})
    list(APPEND LINK_FLAGS ${UBSAN_FLAGS})
endif()

if(DEBUG_INFO)
    list(APPEND COMPILE_FLAGS ${DEBUG_FLAGS})
endif()

# ================================================================
#  OS + Architecture Detection
# ================================================================
if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(OS_NAME "macOS")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(OS_NAME "Linux")
else()
    set(OS_NAME "Unknown")
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^[xX]86_64$")
    set(ARCH "x86_64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$")
    if(OS_NAME STREQUAL "macOS")
        set(ARCH "AppleSilicon")
    else()
        set(ARCH "ARM")
    endif()
else()
    set(ARCH "Unknown")
endif()

# --- AVX2 test when x86_64 ---
include(CheckCXXSourceCompiles)
if(ARCH STREQUAL "x86_64")
    set(CMAKE_REQUIRED_FLAGS "-mavx2")
    check_cxx_source_compiles("
        #include <immintrin.h>
        int main() { __m256i x = _mm256_set1_epi32(0); (void)x; return 0; }
    " HAS_AVX2)
    unset(CMAKE_REQUIRED_FLAGS)

    if(NOT HAS_AVX2)
        message(FATAL_ERROR "x86_64 system without AVX2 is unsupported.")
    endif()
endif()

# ================================================================
#  Preprocessor Definitions
# ================================================================
add_compile_definitions(CMAKE_EXPORT_COMPILE_COMMANDS)

if(USE_AVX_512)
    add_compile_definitions(USE_AVX_512)
endif()

if(CAPS_SA_QUIET)
    add_compile_definitions(CAPS_SA_QUIET)
endif()

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

# SIMD availability
if(ARCH STREQUAL "x86_64")
    set(NATIVE_SIMD TRUE)
    add_compile_definitions(NATIVE_SIMD)
endif()

# ================================================================
#  Thread Library
# ================================================================
find_package(Threads REQUIRED)

# ================================================================
#  ExternalProject dependencies (Parlay + SIMDe)
# ================================================================
include(ExternalProject)

set(EXT_INCLUDE ${CAPS_SA_ROOT}/external/include)
file(MAKE_DIRECTORY ${EXT_INCLUDE})

# --- Parlaylib ---
# --- Parlaylib ---
set(PARLAY_SOURCE_DIR ${CAPS_SA_ROOT}/external/parlaylib)

# Check if parlaylib directory exists and has the expected structure
if(EXISTS ${PARLAY_SOURCE_DIR}/include/parlay)
    message("Found existing parlaylib at ${PARLAY_SOURCE_DIR}")
    # Copy headers directly without ExternalProject
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
        ${PARLAY_SOURCE_DIR}/include/parlay 
        ${EXT_INCLUDE}/parlay
    )
else()
    message("Fetching parlaylib (if missing)")
    ExternalProject_Add(prj_parlaylib
        DOWNLOAD_DIR   ${CAPS_SA_ROOT}/external
        DOWNLOAD_COMMAND ""
        SOURCE_DIR     ${PARLAY_SOURCE_DIR}
        BUILD_IN_SOURCE TRUE
        INSTALL_DIR    ${CAPS_SA_ROOT}/external
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND cp -rf include/parlay ${EXT_INCLUDE}
        GIT_REPOSITORY https://github.com/cmuparlay/parlaylib.git
        GIT_TAG        main
        GIT_SHALLOW    TRUE
    )
    
    # Ensure caps_sa only builds *after* parlaylib is fetched/copied
    add_dependencies(caps_sa prj_parlaylib)
endif()

# --- SIMDe (only when native SIMD unavailable) ---
if(NOT NATIVE_SIMD)
    set(SIMDE_MIN_VERSION "0.8.2")
    message("Fetching SIMDe (non-native SIMD fallback)")
    ExternalProject_Add(prj_simde
        DOWNLOAD_DIR        ${CAPS_SA_ROOT}/external
        DOWNLOAD_COMMAND    curl -k -L https://github.com/simd-everywhere/simde/releases/download/v${SIMDE_MIN_VERSION}/simde-amalgamated-${SIMDE_MIN_VERSION}.tar.xz
                             -o simde-amalgamated-${SIMDE_MIN_VERSION}.tar.xz &&
                             tar -xf simde-amalgamated-${SIMDE_MIN_VERSION}.tar.xz &&
                             rm simde-amalgamated-${SIMDE_MIN_VERSION}.tar.xz
        SOURCE_DIR          ${CAPS_SA_ROOT}/external/simde-amalgamated-${SIMDE_MIN_VERSION}
        BUILD_IN_SOURCE     TRUE
        INSTALL_DIR         ${CAPS_SA_ROOT}/external
        CONFIGURE_COMMAND   ""
        BUILD_COMMAND       ""
        INSTALL_COMMAND     cp -rf x86 ${EXT_INCLUDE}
    )
endif()

# ================================================================
#  Library target
# ================================================================
add_library(caps_sa)

# Auto-add sources
file(GLOB_RECURSE CAPS_SA_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
target_sources(caps_sa PRIVATE ${CAPS_SA_SOURCES})

# Apply compile/link flags
target_compile_options(caps_sa PRIVATE ${COMPILE_FLAGS})
target_link_options(caps_sa PRIVATE ${LINK_FLAGS})

# C++ standard
target_compile_features(caps_sa PUBLIC cxx_std_17)

# Propagate compile definitions to consumers
if(ARCH STREQUAL "x86_64")
    target_compile_definitions(caps_sa PUBLIC NATIVE_SIMD)
endif()
target_compile_definitions(caps_sa PUBLIC L1_CACHE_LINE_SIZE=${L1_CACHE_LINE_SIZE})

# Thread dependency
target_link_libraries(caps_sa PUBLIC Threads::Threads)

# Sync library build with external projects
if(NOT NATIVE_SIMD)
    add_dependencies(caps_sa prj_simde)
endif()

# Include dirs
target_include_directories(caps_sa
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${EXT_INCLUDE}>
        $<INSTALL_INTERFACE:include>
)

# ================================================================
#  Installation + Package Export
# ================================================================
include(GNUInstallDirs)

install(TARGETS caps_sa
    EXPORT caps_saTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY ${EXT_INCLUDE}/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT caps_saTargets
    FILE caps_saConfig.cmake
    NAMESPACE caps_sa::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/caps_sa
)

add_library(caps_sa::caps_sa ALIAS caps_sa)
