#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "input.h"
#include "macros.h"
#include "timer.h"

typedef double value_t;
typedef struct {
    value_t time;
    value_t distance;
} race_t;

#define MAX_RACES 4

typedef struct {
    value_t result;
    race_t races[MAX_RACES];
    int num_entries;
} context_t;

void handle_line(const char *line, int line_num, void* ctx) {
    context_t* context = (context_t *) ctx;
    const char *start;
    char *end;
    if (line_num == 1) {
        context->num_entries = 0;
        start = line + 5;
        while(*start == ' ') start++;
        while(1) {
            context->races[context->num_entries].time = (int) strtol(start, &end, 10);
            context->num_entries++;
            start = end + 1;
            if (*end == '\0') break;
            ASSERT(context->num_entries < MAX_RACES, "Too many races, increase allocation");
        }
    } else {
        context->num_entries = 0;
        start = line + 9;
        while(*start == ' ') start++;
        while(1) {
            context->races[context->num_entries].distance = (int) strtol(start, &end, 10);
            start = end + 1;
            context->num_entries++;
            if (*end == '\0') break;
            ASSERT(context->num_entries < MAX_RACES, "Too many races, increase allocation");
        }
    }
}
value_t parse_ignore_non_digits(const char *in) {
    value_t r = 0;
    while(*in != '\0') {
        if (isdigit(*in)) {
            r = r * 10 + *in - '0';
        }
        in++;
    }
    return r;
}

void handle_line_pt2(const char *line, int line_num, void* ctx) {
    context_t* context = (context_t *) ctx;
    if (line_num == 1) {
        context->races[0].time = parse_ignore_non_digits(line);
    } else {
        context->races[0].distance = parse_ignore_non_digits(line);
        context->num_entries = 1;
    }
}

value_t calc_race_win_options(race_t* r) {
    /* Naive iteration .. maybe too slow on old iron */
    value_t time, distance;
    value_t wins = 0;
    for(time = 1; time < r->time; time++) {
        distance = time * (r->time - time);
        if (distance > r->distance) wins++;
    }
    return wins;
}

value_t calc_race_win_options_quad(race_t* r) {
    /* Use quadratic to find the intercepts for distance on time */
	value_t f = r->time * r->time - 4.0 * r->distance;
	double rv = sqrt(f);
	double x1 = floor((r->time - rv) / 2.0);
	double x2 = ceil((r->time + rv) / 2.0);
    return (value_t) (x2 - x1 - 1);
}

void calc_pt1(context_t* context) {
    int i;
    context->result = 1;
    for (i = 0; i < context->num_entries; i++) {
		context->result *= calc_race_win_options_quad(&context->races[i]);
    }
}

void handle_error(const char *msg, void* ctx) {
    ASSERT(0, msg);
}

int main() {
    time_t clock;
    context_t context;
    const char *sample = "Time:      7  15   30\n"
                         "Distance:  9  40  200";

    const char *puzzle = "Time:        53     89     76     98\n"
                         "Distance:   313   1090   1214   1201";


	context.result = 0;
    timer_start(&clock);
    sample_input(sample, handle_line, &context);
    calc_pt1(&context);
    time_end(&clock, "[PART1:SAMPLE]");
	printf("result: %f\n", context.result);
	ASSERT_EQUAL(context.result, 288, "Part 1 sample answer did not match");

    context.result = 0;
    timer_start(&clock);
    sample_input(puzzle, handle_line, &context);
    calc_pt1(&context);
    time_end(&clock, "[PART1:INPUT]");
	printf("result: %f\n", context.result);
    ASSERT_EQUAL(context.result, 5133600, "Part 1 input answer did not match");

	context.result = 0;
    timer_start(&clock);
    sample_input(sample, handle_line_pt2, (void *) &context);
    time_end(&clock, "[PART2:SAMPLE]");
    calc_pt1(&context);
	printf("result: %f\n", context.result);
    ASSERT_EQUAL(context.result,  71503, "Part 2 sample answer did not match");

    context.result = 0;
    timer_start(&clock);
    sample_input(puzzle, handle_line_pt2, &context);
    calc_pt1(&context);
    time_end(&clock, "[PART2:INPUT]");
	printf("result: %f\n", context.result);
    ASSERT_EQUAL(context.result,  40651271, "Part 2 input answer did not match");

    return 0;
}

