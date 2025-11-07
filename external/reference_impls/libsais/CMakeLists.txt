cmake_minimum_required(VERSION 3.12)
project(reference_libsais)

find_package(OpenMP REQUIRED)

add_library(
        reference_libsais
        libsais.c
        libsais64.c
)

target_include_directories(
        reference_libsais PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_compile_definitions(reference_libsais PUBLIC LIBSAIS_OPENMP)
target_link_libraries(reference_libsais PRIVATE OpenMP::OpenMP_C)