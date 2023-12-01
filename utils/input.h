#ifndef AOC_2023_INPUT_H
#define AOC_2023_INPUT_H

typedef void (*cb_on_line)(const char *line);
typedef void (*cb_on_token)(const char *start, const char *end);
typedef void (*cb_on_error)(const char *msg);

void read_input(const char *filename, cb_on_line on_line, cb_on_error on_error);
void split_line(const char *line, const char *delims, cb_on_token on_token);
#endif