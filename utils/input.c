#include <stdio.h>
#include <string.h>
#include "input.h"

void read_input(const char *filename, cb_on_line on_line, cb_on_error on_error)
{
    char *ptr, buffer[100];
    FILE *in;
    if ((in = fopen(filename, "rt")) != NULL) {
        while((ptr = fgets(buffer, sizeof(buffer) - 1, in)) != NULL)
            on_line(ptr);
        fclose(in);
    } else {
        on_error("Unable to open input file");
    }
}

void split_line(const char *line, const char *delims, cb_on_token on_token)
{
    const char *start = line;
    const char *ptr = start;
    while(*ptr != '\0') {
        if (strchr(delims, *ptr) != NULL) {
            on_token(start, ptr);
            start = ptr + 1;
        }
        ptr++;
    }
    if (start < ptr) on_token(start, ptr);
}