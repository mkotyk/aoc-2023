#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "input.h"

void read_input(const char *filename, cb_on_line on_line, cb_on_error on_error, void* ctx) {
    char *ptr, buffer[512];
    FILE *in;
    int line_num = 1;
    int line_length;
    if ((in = fopen(filename, "rt")) != NULL) {
        while((ptr = fgets(buffer, sizeof(buffer) - 1, in)) != NULL) {
            line_length = strlen(buffer);
            if (buffer[line_length - 1] == '\n')
                buffer[line_length - 1] = '\0';
            on_line(ptr, line_num, ctx);
            line_num++;
        }
        fclose(in);
    } else {
        on_error("Unable to open input file", ctx);
    }
}

char *read_fully(const char *filename) {
    FILE *in;
    char *result = NULL;
    long size;
    if ((in = fopen(filename, "rt")) != NULL) {
        fseek(in, 0L, SEEK_END);
        size = ftell(in);
        fseek(in, 0L, SEEK_SET);
        result = malloc(sizeof(char) * size + 1);
        if (result != NULL) {
            fread(result, sizeof(char), size, in);
        }
        result[size] = '\0';
        fclose(in);
    }
    return result;
}

char *strdup(const char *src) {
	char *copy = malloc(strlen(src) + 1);
	if (copy != NULL) {
		strcpy(copy, src);
	}
	return copy;
}

void sample_input(const char *input, cb_on_line on_line, void* ctx) {
    char *copy, *ptr, *start;
    int line_num = 1;
    copy = strdup(input);
    if (copy != NULL) {
        start = copy;
        while((ptr = strchr(start, '\n')) != NULL) {
            *ptr = '\0';
			ptr++;
			on_line(start, line_num, ctx);
			start = ptr;
			line_num++;
		}
		on_line(start, line_num, ctx);
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
