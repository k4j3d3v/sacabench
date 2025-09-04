cmake_minimum_required(VERSION 3.14)

# Declare the project
project(caps_sa VERSION 0.1.0 LANGUAGES CXX)

# ======================
# 1. Create the library target
# ======================
# We create a library named `caps_sa`. At this point it has no sources yet.
add_library(caps_sa)

# Collect all .cpp source files inside src/ and add them to the target.
# CONFIGURE_DEPENDS ensures CMake re-scans if new .cpp files are added.
file(GLOB_RECURSE CAPS_SA_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

target_sources(caps_sa PRIVATE ${CAPS_SA_SOURCES})

# ======================
# 2. Language standard
# ======================
# Require C++17 and make sure consumers also get that requirement.
target_compile_features(caps_sa PUBLIC cxx_std_17)

# ======================
# 3. Compiler flags
# ======================
# Add warnings and optimizations (PRIVATE so they don't leak to projects using caps_sa).
target_compile_options(caps_sa PRIVATE
    -Wall -Wextra -Wpedantic              # strict warnings
    -Wno-self-assign-overloaded           # suppress specific warning
    -Ofast -mavx2 -funroll-loops -march=native   # aggressive optimizations
)

# ======================
# 4. Sanitizers (optional)
# ======================
if(ASAN) # Address Sanitizer
    target_compile_options(caps_sa PRIVATE -fsanitize=address -fno-omit-frame-pointer -ggdb)
    target_link_options(caps_sa PRIVATE -fsanitize=address)
endif()

if(UBSAN) # Undefined Behavior Sanitizer
    target_compile_options(caps_sa PRIVATE
        -fsanitize=undefined
        -fsanitize=float-divide-by-zero
        -fsanitize=implicit-conversion
        -fsanitize=local-bounds
        -fsanitize=nullability
        -ggdb
    )
    target_link_options(caps_sa PRIVATE -fsanitize=undefined)
endif()

# ======================
# 5. Threads dependency
# ======================
# Find and link the system's thread library (pthread on Linux, etc).
find_package(Threads REQUIRED)
target_link_libraries(caps_sa PUBLIC Threads::Threads)

# ======================
# 6. External dependency: parlaylib
# ======================
# We use ExternalProject_Add to fetch parlaylib from GitHub.
include(ExternalProject)

# Where to copy parlay's headers inside the build tree.
set(PARLAY_BUILD_INCLUDE ${CMAKE_CURRENT_BINARY_DIR}/parlay)
file(MAKE_DIRECTORY ${PARLAY_BUILD_INCLUDE})

message("Build system will fetch and install parlaylib")

ExternalProject_Add(prj_parlaylib
    DOWNLOAD_DIR        ${CMAKE_CURRENT_SOURCE_DIR}/external              # Download location
    DOWNLOAD_COMMAND    git clone https://github.com/cmuparlay/parlaylib.git
    SOURCE_DIR          ${CMAKE_CURRENT_SOURCE_DIR}/external/parlaylib    # Where source lives
    BUILD_IN_SOURCE     TRUE
    INSTALL_DIR         ${CMAKE_CURRENT_BINARY_DIR}                       # Install location
    CONFIGURE_COMMAND   ""    # No configure step
    BUILD_COMMAND       ""    # No build step (header-only)
    INSTALL_COMMAND     cp -rf include/parlay ${PARLAY_BUILD_INCLUDE}     # Copy headers to build dir
)

# Ensure caps_sa only builds *after* parlaylib is fetched/copied
add_dependencies(caps_sa prj_parlaylib)

# ======================
# 7. Include directories
# ======================
# - BUILD_INTERFACE = used when building inside this source tree
# - INSTALL_INTERFACE = used when someone installs/uses this library via find_package
target_include_directories(caps_sa
    PUBLIC
        # Our project headers
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>

        # Parlay headers during build
        $<BUILD_INTERFACE:${PARLAY_BUILD_INCLUDE}>
        # Parlay headers after install
        $<INSTALL_INTERFACE:include/parlay>
)

# ======================
# 8. Install/export
# ======================
include(GNUInstallDirs)

# Install the caps_sa library
install(TARGETS caps_sa
    EXPORT caps_saTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}   # .a (static lib)
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}   # .so/.dylib (shared lib)
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}   # .dll (Windows runtime)
)

# Install our public headers from include/
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Install parlay headers alongside
install(DIRECTORY ${PARLAY_BUILD_INCLUDE}/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/parlay)

# Export CMake package config so other projects can `find_package(caps_sa CONFIG)`
install(EXPORT caps_saTargets
    FILE caps_saConfig.cmake
    NAMESPACE caps_sa::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/caps_sa
)

# Create an ALIAS target so consumers can use `caps_sa::caps_sa`
add_library(caps_sa::caps_sa ALIAS caps_sa)
