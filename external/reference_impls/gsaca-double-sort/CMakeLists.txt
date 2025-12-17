# add_library(reference_dsh INTERFACE)

# target_include_directories(
#         reference_dsh INTERFACE
#         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
#         $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
# )
# Create header-only library
add_library(dsh INTERFACE)
set(CMAKE_CXX_STANDARD 17)
set(WARNING_FLAGS -Wall -Wextra -Wpedantic)
set(OPTIMIZE_FLAGS -O3 -funroll-loops -march=native)

# Include directories for header-only library
target_include_directories(dsh
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# Add compile and link flags
target_compile_options(dsh INTERFACE
    ${SACA_BENCH_FLAGS})

# target_link_libraries(dsh INTERFACE
#     atomic     # equivalent to -latomic
#     OpenMP::OpenMP_CXX   # preferred over raw -fopenmp if find_package(OpenMP) is used
# )

# Compiler features
target_compile_features(dsh INTERFACE cxx_std_17)
