#include <stdio.h>
#include <stdlib.h>
#include "../utils/input.h"

int word_count = 0;

void handle_token(const char *token, const char *end) {
    word_count++;
}

void handle_input(const char *line) {
    split_line(line, " ", handle_token);
}

void handle_error(const char *msg) {
    fputs(msg, stderr);
    exit(-1);
}

int main() {
    read_input("../day1/input.txt", handle_input, handle_error);
    printf("Word count: %d\n", word_count);
    printf("Hello, World!\n");
    return 0;
}
