#include <stdio.h>
#include <stdlib.h>
#include "input.h"
#include "macros.h"
#include "timer.h"

#define MAP_WIDTH   140
#define MAP_HEIGHT  140

enum {
    UP = 1,
    LEFT = 2,
    DOWN = 4,
    RIGHT = 8
};

typedef long value_t;
typedef struct {
    int x;
    int y;
} point_t;

typedef struct {
    value_t result;
    char map[MAP_HEIGHT][MAP_WIDTH];
    int dist_map[MAP_HEIGHT][MAP_WIDTH];
    int width;
    int height;
    point_t start;
} context_t;

void handle_line(const char *line, int line_num, void *ctx) {
    context_t *context = (context_t *) ctx;
    const char *ptr;
    memcpy(context->map[line_num - 1], line, strlen(line));
    if ((ptr = strchr(line, 'S')) != NULL) {
        context->start.x = ptr - line;
        context->start.y = line_num - 1;
    }
    if (context->width < 0) context->width = strlen(line);
    if (line_num > context->height) context->height = line_num;
}

void handle_error(const char *msg, void *ctx) {
    ASSERT(0, msg);
}

char read_map(context_t *context, point_t *p) {
    if (p->x < 0 || p->x > context->width) return -1;
    if (p->y < 0 || p->y > context->height) return -1;
    return context->map[p->y][p->x];
}

int read_dist(context_t *context, point_t *p) {
    if (p->x < 0 || p->x > context->width) return -1;
    if (p->y < 0 || p->y > context->height) return -1;
    return context->dist_map[p->y][p->x];
}

void write_dist(context_t *context, point_t *p, int steps) {
    if (p->x < 0 || p->x > context->width) return;
    if (p->y < 0 || p->y > context->height) return;
    context->dist_map[p->y][p->x] = steps;
}

int in(const char c, const char *in, size_t s) {
    size_t i;
    for (i = 0; i < s; i++) if (in[i] == c) return 1;
    return 0;
}

int move_path(point_t *p, int steps, context_t *context) {
    char c;
    point_t np, inside_p;
    point_t movements[4] = {
            {0,  -1}, /* Up */
            {-1, 0}, /* Left */
            {0,  1}, /* Down */
            {1,  0} /* Right */
    };
    const char *valid_pipes[4] = {
            "|7F", /* Up */
            "-LF", /* Left */
            "|LJ", /* Down */
            "-7J" /* Right */
    };
    int i, valid = 0, facing;

    switch (read_map(context, p)) {
        case 'S':
            valid = LEFT | DOWN | UP | RIGHT;
            break;
        case '7':
            valid = LEFT | DOWN;
            break;
        case 'J':
            valid = UP | LEFT;
            break;
        case 'F':
            valid = DOWN | RIGHT;
            break;
        case 'L':
            valid = UP | RIGHT;
            break;
        case '|':
            valid = UP | DOWN;
            break;
        case '-':
            valid = LEFT | RIGHT;
            break;
    }

    ASSERT(valid, "No valid direction");

    for (i = 0; i < NELEM(movements); i++) {
        if (!((1 << i) & valid)) continue;
        np = *p;
        np.x += movements[i].x;
        np.y += movements[i].y;
        c = read_map(context, &np);
        if (in(c, valid_pipes[i], 3) && read_dist(context, &np) == 0) {
            write_dist(context, &np, steps);
            *p = np;
            return 1;
        }
    }

    /* Nowhere to move */
    return 0;
}

void walk_map(context_t *context) {
    point_t p = context->start;
    write_dist(context, &context->start, 1);
    int steps = 1;
    while (1) {
        if (!move_path(&p, steps, context)) break;
        steps++;
    }

    context->result = steps / 2;
}

void calc_inside(context_t *context) {
    int x, y, inside;

    context->result = 0;
    for (y = 0; y < context->height; y++) {
        inside = 0;
        for (x = 0; x < context->width; x++) {
            if (context->dist_map[y][x] != 0) {
                if (in(context->map[y][x], "|F7S", 4))
                    inside = !inside;
            } else {
                if (inside) context->result++;
            }
        }
    }
}

void init_context(context_t *context) {
    context->result = 0;
    memset(context->map, 0, sizeof(context->map));
    memset(context->dist_map, 0, sizeof(context->dist_map));
    context->width = -1;
    context->height = -1;
}

int main() {
    time_t clock;
    context_t context;
    const char *sample = "..F7.\n"
                         ".FJ|.\n"
                         "SJ.L7\n"
                         "|F--J\n"
                         "LJ...";

    const char *sample2 = ".F----7F7F7F7F-7....\n"
                          ".|F--7||||||||FJ....\n"
                          ".||.FJ||||||||L7....\n"
                          "FJL7L7LJLJ||LJ.L-7..\n"
                          "L--J.L7...LJS7F-7L7.\n"
                          "....F-J..F7FJ|L7L7L7\n"
                          "....L7.F7||L7|.L7L7|\n"
                          ".....|FJLJ|FJ|F7|.LJ\n"
                          "....FJL-7.||.||||...\n"
                          "....L---J.LJ.LJLJ...";

    const char *sample3 = "FF7FSF7F7F7F7F7F---7\n"
                          "L|LJ||||||||||||F--J\n"
                          "FL-7LJLJ||||||LJL-77\n"
                          "F--JF--7||LJLJ7F7FJ-\n"
                          "L---JF-JLJ.||-FJLJJ7\n"
                          "|F|F-JF---7F7-L7L|7|\n"
                          "|FFJF7L7F-JF7|JL---7\n"
                          "7-L-JL7||F7|L7F-7F7|\n"
                          "L.L7LFJ|||||FJL7||LJ\n"
                          "L7JLJL-JLJLJL--JLJ.L";


    init_context(&context);
    timer_start(&clock);
    sample_input(sample, handle_line, &context);
    walk_map(&context);
    time_end(&clock, "[PART1:SAMPLE]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 8, "Part 1 sample answer did not match");

    init_context(&context);
    timer_start(&clock);
    read_input("day10.txt", handle_line, handle_error, &context);
    walk_map(&context);
    time_end(&clock, "[PART1:INPUT]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 6701, "Part 1 input answer did not match");

    init_context(&context);
    timer_start(&clock);
    sample_input(sample2, handle_line, (void *) &context);
    walk_map(&context);
    calc_inside(&context);
    time_end(&clock, "[PART2:SAMPLE]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 8, "Part 2 sample answer did not match");

    init_context(&context);
    timer_start(&clock);
    sample_input(sample3, handle_line, (void *) &context);
    walk_map(&context);
    calc_inside(&context);
    time_end(&clock, "[PART2:SAMPLE]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 10, "Part 2 sample answer did not match");

    init_context(&context);
    timer_start(&clock);
    read_input("day10.txt", handle_line, handle_error, &context);
    walk_map(&context);
    calc_inside(&context);
    time_end(&clock, "[PART2:INPUT]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 303, "Part 2 input answer did not match");
    return 0;
}
