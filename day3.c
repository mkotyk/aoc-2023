#include <stdio.h>
#include <stdlib.h>
#include "input.h"
#include "strtools.h"
#include "macros.h"
#include "timer.h"
struct context;
typedef char (*pos_cb)(int,int,struct context*);
const char OOB = -1;

typedef struct {
    int x;
    int y;
} coord_t;

typedef struct {
    int value;
    coord_t coord;
} gear_coord_t;

struct context {
    long result;
    const char *map;
    int width;
    int height;
    pos_cb pos;
    gear_coord_t gears[1000];
    int num_gears;
};
typedef struct context context_t;

int is_digit(char c) {
    return c >= '0' && c <= '9';
}

int is_part(char c) {
    return c != OOB && !is_digit(c) && c != '.' && c != '\n';
}

char char_at_position(int x, int y, context_t* ctx) {
    if (x < 0 || x >= ctx->width) return OOB;
    if (y < 0 || y >= ctx->height) return OOB;
    int offset = x + y * ctx->width;
    return *(ctx->map + offset);
}

typedef enum {
    SCANNING,
    ON_NUMBER,
} state_t;

void measure(context_t* ctx) {
    const char *ptr, *start = ctx->map;
    int count = 1;
    ASSERT(start != NULL, "Null input");
    while((ptr = strchr(start, '\n'))!= NULL) {
        count++;
        if (ctx->width == 0) {
            ctx->width = (int)(ptr - start) + 1;
        }
        start = ptr + 1;
    }
    ctx->height = count;
}

void scan_parts(context_t* ctx) {
    int x,y,i,j;
    char cell, part;
    state_t state = SCANNING;
    int value = 0, valid = 0;
    coord_t near_gear[10];
    int near_gear_count = 0, index;

    for(y = 0; y < ctx->height; y++) {
        for(x = 0; x < ctx->width; x++) {
            cell = ctx->pos(x, y, ctx);
            switch(state) {
                case SCANNING:
                    if (is_digit(cell)) {
                        state = ON_NUMBER;
                        valid = 0;
                        value = 0;
                    }
                case ON_NUMBER:
                    if (is_digit(cell)) {
                        value = value * 10 + cell - '0';
                        for(i = -1; i < 2 && !valid; i++) {
                            for(j = -1; j < 2 && !valid; j++) {
                                part = ctx->pos(x+i, y+j, ctx);
                                valid |= is_part(part);
                                if (part == '*') {
                                    near_gear[near_gear_count].x = x+i;
                                    near_gear[near_gear_count].y = y+j;
                                    near_gear_count++;
                                }
                            }
                        }
                    } else {
                        state = SCANNING;
                        if (valid) {
                            ctx->result += value;
                            for(index = 0; index < near_gear_count; index++) {
                                ctx->gears[ctx->num_gears].value = value;
                                ctx->gears[ctx->num_gears].coord = near_gear[index];
                                ctx->num_gears++;
                            }
                            near_gear_count = 0;
                        }
                        value = 0; valid = 0;
                    }
                    break;
            }
        }
    }
}

void calc_ratios(context_t* ctx) {
    int i,j;
    ctx->result = 0;
    for(i = 0; i < ctx->num_gears; i++) {
        for(j = i + 1; j < ctx->num_gears; j++) {
            if ((ctx->gears[i].coord.x == ctx->gears[j].coord.x) &&
                (ctx->gears[i].coord.y == ctx->gears[j].coord.y)) {
                ctx->result += ctx->gears[i].value * ctx->gears[j].value;
            }
        }
    }
}

void handle_error(const char *msg, void* ctx) {
    ASSERT(0, msg);
}

int main() {
    time_t clock;
    context_t context;
    const char *sample_pt1 = "467..114..\n"
                             "...*......\n"
                             "..35..633.\n"
                             "......#...\n"
                             "617*......\n"
                             ".....+.58.\n"
                             "..592.....\n"
                             "......755.\n"
                             "...$.*....\n"
                             ".664.598..";

    context.pos = char_at_position;
    context.width = context.height = 0;
    context.num_gears = 0;
    context.result = 0;
    timer_start(&clock);
    context.map = sample_pt1;
    measure(&context);
    scan_parts(&context);
    time_end(&clock, "[PART1:SAMPLE]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 4361, "Part 1 sample answer did not match");

    context.result = 0;
    context.width = context.height = 0;
    context.num_gears = 0;
    timer_start(&clock);
    context.map = read_fully("day3.txt");
    measure(&context);
    scan_parts(&context);
    free(context.map);
    time_end(&clock, "[PART1:INPUT]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 521601, "Part 1 input answer did not match");

    context.result = 0;
    context.width = context.height = 0;
    context.num_gears = 0;
    timer_start(&clock);
    context.map = sample_pt1;
    measure(&context);
    scan_parts(&context);
    calc_ratios(&context);
    time_end(&clock, "[PART2:SAMPLE]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 467835, "Part 2 sample answer did not match");

    context.result = 0;
    context.width = context.height = 0;
    context.num_gears = 0;
    timer_start(&clock);
    context.map = read_fully("day3.txt");
    measure(&context);
    scan_parts(&context);
    calc_ratios(&context);
    free(context.map);
    time_end(&clock, "[PART2:INPUT]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 80694070, "Part 2 input answer did not match");

    return 0;
}