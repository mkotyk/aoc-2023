#ifndef AOC_2023_INPUT_H
#define AOC_2023_INPUT_H

typedef void (*cb_on_line)(const char *line, int line_num, void* ctx);
typedef void (*cb_on_token)(const char *start, const char *end, void* ctx);
typedef void (*cb_on_error)(const char *msg, void* ctx);

void read_input(const char *filename, cb_on_line on_line, cb_on_error on_error, void* ctx);
void sample_input(const char *input, cb_on_line on_line, void* ctx);
void split_line(const char *line, const char *delims, cb_on_token on_token, void* ctx);

#endif