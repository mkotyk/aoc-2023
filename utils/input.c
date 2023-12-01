#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "input.h"

void read_input(const char *filename, cb_on_line on_line, cb_on_error on_error, void* ctx) {
    char *ptr, buffer[100];
    FILE *in;
    int line_num = 1;
    if ((in = fopen(filename, "rt")) != NULL) {
        while((ptr = fgets(buffer, sizeof(buffer) - 1, in)) != NULL) {
            on_line(ptr, line_num, ctx);
            line_num++;
        }
        fclose(in);
    } else {
        on_error("Unable to open input file", ctx);
    }
}

void sample_input(const char *input, cb_on_line on_line, void* ctx) {
    char *copy, *ptr, *start;
    int line_num = 1;
    if ((copy = strdup(input)) != NULL) {
        start = copy;
        while((ptr = strsep(&start, "\n"))) {
            on_line(ptr, line_num, ctx);
            line_num++;
        }
        free(copy);
    }
}

void split_line(const char *line, const char *delims, cb_on_token on_token, void* ctx) {
    const char *start = line;
    const char *ptr = start;
    while(*ptr != '\0') {
        if (strchr(delims, *ptr) != NULL) {
            on_token(start, ptr, ctx);
            start = ptr + 1;
        }
        ptr++;
    }
    if (start < ptr) on_token(start, ptr, ctx);
}