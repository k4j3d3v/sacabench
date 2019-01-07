#include <cuda.h>

void cuda_check_internal(char const* file, int line, cudaError v, char const* reason = "");

#define cuda_check(...) cuda_check_internal(__FILE__, __LINE__, __VA_ARGS__)
