#include <stdio.h>
#include <stdlib.h>
#include "input.h"
#include "strtools.h"
#include "macros.h"
#include "timer.h"
#define MAX_INPUT   100
#define MAX_LAYERS  20

typedef long value_t;
typedef value_t (*cb_predictor)(value_t* input, int size);
typedef struct {
    value_t result;
    cb_predictor predictor;
} context_t;

value_t predict_next(value_t* input, int size) {
    value_t layers[MAX_LAYERS][MAX_INPUT];
    int i, j, all_zero;

    memset(&layers, 0, sizeof(layers));
    memcpy(&layers[0], input, sizeof(value_t) * size);
    for(j = 0; j < MAX_LAYERS; j++) {
        all_zero = 1;
        for (i = 0; i < size - (j + 1); i++) {
            layers[j + 1][i] = layers[j][i + 1] - layers[j][i];
            all_zero &= (layers[j + 1][i] == 0);
        }
        if (all_zero) break;
    }

    for(i = j + 1; i; i--) {
        layers[i - 1][size - (i - 1)] = layers[i][size - i] + layers[i-1][size - i];
    }

    return layers[0][size];
}

value_t predict_prev(value_t* input, int size) {
    value_t layers[MAX_LAYERS][MAX_INPUT], diff;
    int i, j, all_zero;

    memset(&layers, 0, sizeof(layers));
    memcpy(&layers[0][1], input, sizeof(value_t) * size);
    for(j = 0; j < MAX_LAYERS; j++) {
        all_zero = 1;
        for (i = size - (j + 1); i > 0; i--) {
            diff = layers[j][i + 1] - layers[j][i];
            layers[j + 1][i] = diff;
            all_zero &= (diff == 0);
        }
        if (all_zero) break;
    }

    for(i = j + 1; i; i--) {
        layers[i - 1][0] = layers[i-1][1] - layers[i][0] ;
    }

    return layers[0][0];
}

void handle_input(const char *line, int line_num, void* ctx) {
    context_t* context = (context_t *) ctx;
    const char *start = line;
    char *end;
    value_t input[MAX_INPUT];
    int num_input;

    num_input = 0;
    while(1) {
        input[num_input++] = strtol(start, &end, 10);
        if (*end == '\0') break;
        start = end + 1;
    }

    context->result += context->predictor(input, num_input);
}

void handle_error(const char *msg, void* ctx) {
    ASSERT(0, msg);
}

int main() {
    time_t clock;
    context_t  context;
    const char *sample = "0 3 6 9 12 15\n"
                         "1 3 6 10 15 21\n"
                         "10 13 16 21 30 45";

    context.result = 0;
    context.predictor = predict_next;
    timer_start(&clock);
    sample_input(sample, handle_input, &context);
    time_end(&clock, "[PART1:SAMPLE]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 114, "Part 1 sample answer did not match");

    context.result = 0;
    context.predictor = predict_next;
    timer_start(&clock);
    read_input("day9.txt", handle_input, handle_error, &context);
    time_end(&clock, "[PART1:INPUT]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 1743490457, "Part 1 input answer did not match");

    context.result = 0;
    context.predictor = predict_prev;
    timer_start(&clock);
    sample_input(sample, handle_input, (void *) &context);
    time_end(&clock, "[PART2:SAMPLE]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result,  2, "Part 2 sample answer did not match");

    context.result = 0;
    context.predictor = predict_prev;
    timer_start(&clock);
    read_input("day9.txt", handle_input, handle_error, &context);
    time_end(&clock, "[PART2:INPUT]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result,  53592, "Part 2 input answer did not match");

    return 0;
}

