#include <stdio.h>
#include <stdlib.h>
#include "input.h"
#include "macros.h"
#include "timer.h"

#define MAP_WIDTH   140
#define MAP_HEIGHT  140
#define MAX_GALAXIES 1024

typedef long value_t;
typedef struct {
    value_t x;
    value_t y;
} point_t;

typedef struct {
    value_t result;
    char map[MAP_HEIGHT][MAP_WIDTH];
    int blank_columns[MAP_WIDTH];
    int num_blank_columns;
    int blank_rows[MAP_HEIGHT];
    int num_blank_rows;
    point_t galaxies[MAX_GALAXIES];
    int num_galaxies;
    int width;
    int height;
    point_t start;
} context_t;

void handle_line(const char *line, int line_num, void *ctx) {
    context_t *context = (context_t *) ctx;
    int len = strlen(line);
    memcpy(context->map[line_num - 1], line, len);
    if (context->width < 0) context->width = len;
    if (line_num > context->height) context->height = line_num;
}

void handle_error(const char *msg, void *ctx) {
    ASSERT(0, msg);
}

void init_context(context_t *context) {
    context->result = 0;
    memset(context->map, 0, sizeof(context->map));
    memset(context->blank_columns, 0, sizeof(context->blank_columns));
    memset(context->blank_rows, 0, sizeof(context->blank_rows));
    context->num_blank_columns = 0;
    context->num_blank_rows = 0;
    context->num_galaxies = 0;
    context->width = -1;
    context->height = -1;
}

int is_blank_row(context_t* context, int row) {
    int i;
    for(i = 0; i < context->num_blank_rows; i++) {
        if (context->blank_rows[i] == row) return 1;
    }
    return 0;
}

int is_blank_col(context_t* context, int col) {
    int i;
    for(i = 0; i < context->num_blank_columns; i++) {
        if (context->blank_columns[i] == col) return 1;
    }
    return 0;
}

void expand_map_to_points(context_t* context, value_t dist) {
    int x,y;
    value_t i,j;
    context->num_galaxies = 0;
    for(y = 0, j = 0; y < context->height; y++, j++) {
        if (is_blank_row(context, y)) j += dist;
        for(x = 0, i = 0; x < context->width; x++, i++) {
            if (is_blank_col(context, x)) i += dist;
            if (context->map[y][x] == '#') {
                context->galaxies[context->num_galaxies].x = i;
                context->galaxies[context->num_galaxies++].y = j;
            }
        }
    }
}

void detect_blank_rows(context_t* context) {
    int x,y;
    context->num_blank_rows = 0;
    for(y = 0; y < context->height; y++) {
        for(x = 0; x < context->width; x++) {
            if (context->map[y][x] != '.') break;
        }
        if (x == context->width) {
            context->blank_rows[context->num_blank_rows++] = y;
        }
    }
}

void detect_blank_columns(context_t* context) {
    int x,y;
    context->num_blank_columns = 0;
    for(x = 0; x < context->width; x++) {
        for(y = 0; y < context->height; y++) {
            if (context->map[y][x] != '.') break;
        }
        if (y == context->height) {
            context->blank_columns[context->num_blank_columns++] = x;
        }
    }
}

value_t distance(point_t p1, point_t p2) {
    return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

void calc(context_t* context, value_t dist) {
    int i,j;

    detect_blank_rows(context);
    detect_blank_columns(context);
    expand_map_to_points(context, dist);

    context->result = 0;
    for(i = 0; i < context->num_galaxies; i++) {
        for(j = i + 1; j < context->num_galaxies; j++) {
            context->result += distance(context->galaxies[i], context->galaxies[j]);
        }
    }
}

int main() {
    time_t clock;
    context_t context;
    const char *sample = "...#......\n"
                         ".......#..\n"
                         "#.........\n"
                         "..........\n"
                         "......#...\n"
                         ".#........\n"
                         ".........#\n"
                         "..........\n"
                         ".......#..\n"
                         "#...#.....";

    init_context(&context);
    timer_start(&clock);
    sample_input(sample, handle_line, &context);
    calc(&context, 1);
    time_end(&clock, "[PART1:SAMPLE]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 374, "Part 1 sample answer did not match");

    init_context(&context);
    timer_start(&clock);
    read_input("day11.txt", handle_line, handle_error, &context);
    calc(&context, 1);
    time_end(&clock, "[PART1:INPUT]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 10276166, "Part 1 input answer did not match");

    init_context(&context);
    timer_start(&clock);
    sample_input(sample, handle_line, (void *) &context);
    calc(&context, 10 - 1);
    time_end(&clock, "[PART2:SAMPLE]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 1030, "Part 2 sample answer did not match");

    init_context(&context);
    timer_start(&clock);
    sample_input(sample, handle_line, (void *) &context);
    calc(&context, 100 - 1);
    time_end(&clock, "[PART2:SAMPLE]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 8410, "Part 2 sample answer did not match");

    init_context(&context);
    timer_start(&clock);
    read_input("day11.txt", handle_line, handle_error, &context);
    calc(&context, 1000000 - 1);
    time_end(&clock, "[PART2:INPUT]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 598693078798, "Part 2 input answer did not match");

    return 0;
}
