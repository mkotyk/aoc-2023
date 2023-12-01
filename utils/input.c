#include <stdio.h>
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