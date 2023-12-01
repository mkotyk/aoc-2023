#include <stdio.h>
#include <stdlib.h>
#include "../utils/input.h"

void handle_input(const char *line) {

}

void handle_error(const char *msg, int errno) {
    fprintf(stderr, "Error: %s Errno:%d\n", msg, errno);
    exit(-1);
}

int main() {
    read_input("day1.txt", handle_input, handle_error);
    printf("Hello, World!\n");
    return 0;
}
