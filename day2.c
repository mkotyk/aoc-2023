#include <stdio.h>
#include <stdlib.h>
#include "input.h"
#include "strtools.h"
#include "macros.h"
#include "timer.h"

typedef struct {
    int red;
    int green;
    int blue;
} cube_num_t;

struct context;
typedef void (*check_round_cb)(struct context* ctx, cube_num_t* round);
typedef void (*game_cb)(struct context *ctx, int game);

struct context {
    cube_num_t* limits;
    long result;
    int valid_round;
    check_round_cb check_round;
    game_cb game_started;
    game_cb game_finished;
};

typedef struct context context_t;

void parse_cube(char *in, cube_num_t* round) {
    char *ptr, *start = in;
    int count;
    if ((ptr = strchr(start, ' ')) != NULL) {
        *ptr++ = '\0';
        count = atoi(start);
        if (strcmp(ptr, "red") == 0) {
            round->red = count;
        } else if (strcmp(ptr, "green") == 0) {
            round->green = count;
        } else if (strcmp(ptr, "blue") == 0) {
            round->blue = count;
        } else {
            ASSERT(0, "Unknown color");
        }
    }
}

void parse_round(char *in, cube_num_t* round) {
    round->red = round->green = round->blue = 0;
    char *ptr, *start = in;
    while(*start == ' ') start++;
    while((ptr = strchr(start, ',')) != NULL) {
        *ptr++ = '\0';
        while(*ptr == ' ') ptr++;
        parse_cube(start, round);
        start = ptr;
    }
    parse_cube(start, round);
}

void round_under_limit(context_t* ctx, cube_num_t* round) {
    ctx->valid_round &= (round->red <= ctx->limits->red) &&
           (round->blue <= ctx->limits->blue) &&
           (round->green <= ctx->limits->green);
}

void round_under_start(context_t* ctx, int game) {
    ctx->valid_round = 1;
}

void round_under_finished(context_t* ctx, int game) {
    if (ctx->valid_round) {
        ctx->result += game;
    }
}

void round_max_values(context_t* ctx, cube_num_t* round) {
    if (round->red > ctx->limits->red)  ctx->limits->red = round->red;
    if (round->green > ctx->limits->green)  ctx->limits->green = round->green;
    if (round->blue > ctx->limits->blue)  ctx->limits->blue = round->blue;
}

void round_max_values_start(context_t* ctx, int game) {
    ctx->limits->red = 0;
    ctx->limits->green = 0;
    ctx->limits->blue = 0;
}

void round_max_values_finished(context_t* ctx, int game) {
    long power = ctx->limits->red *
                 ctx->limits->green *
                 ctx->limits->blue;
    ctx->result += power;
}

void handle_line(const char *line, int line_num, void* ctx) {
    context_t* context = (context_t *) ctx;
    char *copy = strdup(line);
    char *ptr, *start;
    int game = -1;
    char delim = ':';
    int valid_game = 1;
    cube_num_t round;

    if (copy != NULL) {
        start = copy;
        while ((ptr = strchr(start, delim)) != NULL) {
            *ptr++ = '\0';
            switch(delim) {
                case ':':
                    start += 5; /* Skip over "Game " */
                    ASSERT(start < ptr, "Expected Game");
                    game = atoi(start);
                    context->game_started(context, game);
                    delim = ';';
                break;
                case ';':
                    parse_round(start, &round);
                    context->check_round(context, &round);
                    break;
            }
            start = ptr;
        }
        parse_round(start, &round);
        context->check_round(context, &round);
        free(copy);
    }
    context->game_finished(context, game);
}

void handle_error(const char *msg, void* ctx) {
    ASSERT(0, msg);
}

int main() {
    time_t clock;
    cube_num_t cube_limits = {
            .red = 12,
            .green = 13,
            .blue = 14
    };
    context_t context = {
            .limits = &cube_limits,
            .result = 0
    };

    const char *sample_pt1 = "Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green\n"
                         "Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue\n"
                         "Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red\n"
                         "Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red\n"
                         "Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green";

    context.check_round = round_under_limit;
    context.game_started = round_under_start;
    context.game_finished = round_under_finished;

    timer_start(&clock);
    sample_input(sample_pt1, handle_line, &context);
    time_end(&clock, "[PART1:SAMPLE]");
	printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 8, "Part 1 sample answer did not match");

    context.result = 0;
    timer_start(&clock);
    read_input("day2.txt", handle_line, handle_error, &context);
    time_end(&clock, "[PART1:INPUT]");
	printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 2716, "Part 1 input answer did not match");

	context.result = 0;
    context.check_round = round_max_values;
    context.game_started = round_max_values_start;
    context.game_finished = round_max_values_finished;

    timer_start(&clock);
	sample_input(sample_pt1, handle_line, (void *) &context);
    time_end(&clock, "[PART2:SAMPLE]");
	printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result,  2286, "Part 2 sample answer did not match");

    context.result = 0;
    timer_start(&clock);
    read_input("day2.txt", handle_line, handle_error, &context);
    time_end(&clock, "[PART2:INPUT]");
	printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result,  72227, "Part 2 input answer did not match");

    return 0;
}

