#ifndef AOC_2023_STRTOOLS_H
#define AOC_2023_STRTOOLS_H

#include <string.h>
typedef int (*cb_on_found)(const char *pos, const char *needle, int needle_index, void* ctx);

void find_forward(const char** needles, size_t needle_size, const char* haystack, cb_on_found on_found, void* ctx);
void find_backward(const char** needles, size_t needle_size, const char* haystack, cb_on_found on_found, void* ctx);

#endif
