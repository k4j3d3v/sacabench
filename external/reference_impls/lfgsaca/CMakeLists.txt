# Create header-only library
add_library(fgsaca INTERFACE)

# Include directories for header-only library
target_include_directories(fgsaca 
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# Compiler options and features for header-only library
target_compile_options(fgsaca INTERFACE ${SACA_BENCH_FLAGS})
target_compile_features(fgsaca INTERFACE cxx_std_20)

# Optional: Create demo executable
#add_executable(fgsaca_demo main.cpp)
#target_link_libraries(fgsaca_demo PRIVATE fgsaca)
