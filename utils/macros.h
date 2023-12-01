#ifndef AOC_2023_MACROS_H
#define AOC_2023_MACROS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASSERT(c,msg) { if (!(c)) { fputs(msg, stderr); exit(-1); } }
#define ASSERT_EQUAL(result, expected, msg) ASSERT((result) == (expected), msg)
#ifndef NELEM
#  define NELEM(x) (sizeof(x) / sizeof(x[0]))
#endif

#define ENDSTRPTR(x) (x + strlen(x))
#define MIN(a,b) ((a < b)?a:b)
#define MAX(a,b) ((a > b)?a:b)

#endif