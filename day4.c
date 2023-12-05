#include <stdio.h>
#include <stdlib.h>
#include "input.h"
#include "macros.h"
#include "timer.h"
#include "parser.h"

typedef struct {
    int card_num;
	int winning[10];
	int selected[25];
    int matches;
} card_t;

typedef struct {
    unsigned long result;
    int winning_count;
    int selected_count;
	card_t cards[220];
    int card_count;
    expr_t parse_expr;
} context_t;

int range_to_int(range_t* r) {
    int value = 0;
    const char *p = r->start;
    while(p != r->end) value = value * 10 + *p++ - '0';
    return value;
}

int calc_card_matches(context_t* context, card_t* card) {
    int i, j;
    int matches = 0;
    for (i = 0; i < context->winning_count; i++) {
        for (j = 0; j < context->selected_count; j++) {
            if (card->winning[i] == card->selected[j]) matches++;
        }
    }
    return matches;
}

void handle_line(const char *line, int line_num, void* ctx) {
    context_t* context = (context_t *) ctx;
    int i;
    parse_context_t* parse_context = init_parse_context(line, 100);
    if (evaluate(context->parse_expr, parse_context)) {
        context->cards[context->card_count].card_num = range_to_int(&parse_context->captures[0]);
        for(i = 0; i < context->winning_count; i++)
            context->cards[context->card_count].winning[i] = range_to_int(&parse_context->captures[1 + i]);
        for(i = 0; i < context->selected_count; i++)
            context->cards[context->card_count].selected[i] = range_to_int(&parse_context->captures[1 + context->winning_count + i]);
        context->cards[context->card_count].matches = calc_card_matches(context, &context->cards[context->card_count]);
        context->card_count++;
    } else {
        ASSERT(0, "Unable to parse");
    }
    free_parse_context(parse_context);
}

void score_card_pt1(context_t* context) {
    int c;
    for(c = 0; c < context->card_count; c++) {
        if (context->cards[c].matches > 0) {
            context->result += 1 << (context->cards[c].matches - 1);
        }
    }
}

unsigned long score_card_pt2(context_t* context, int start_card, int end_card) {
	int c;
	unsigned long sum = MIN(end_card, context->card_count) - MIN(start_card, context->card_count);
    for(c = start_card; c < end_card && c < context->card_count; c++) {
        if (context->cards[c].matches > 0) {
            sum += score_card_pt2(context, c + 1, c + 1 + context->cards[c].matches);
        }
    }
    return sum;
}

void handle_error(const char *msg, void* ctx) {
    ASSERT(0, msg);
}

int main() {
    time_t clock;
    context_t context;
    const char *sample_pt1 = "Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53\n"
                             "Card 2: 13 32 20 16 61 | 61 30 68 82 17 32 24 19\n"
                             "Card 3:  1 21 53 59 44 | 69 82 63 72 16 21 14  1\n"
                             "Card 4: 41 92 73 84 69 | 59 84 76 51 58  5 54 83\n"
                             "Card 5: 87 83 26 28 32 | 88 30 70 12 93 22 82 36\n"
                             "Card 6: 31 18 13 56 72 | 74 77 10 23 35 67 36 11";
    expr_t card = all(
            start(),
            literal("Card"),
            some(whitespace()),
            capture(number()),
            literal(":"),
            some(all(some(whitespace()), capture(number()), NULL)),
            whitespace(),
            literal("|"),
            some(all(some(whitespace()), capture(number()), NULL)),
            optional(whitespace()),
            end(),
            NULL
    );

    context.parse_expr = card;
    context.winning_count = 5;
    context.selected_count = 8;
    context.card_count = 0;
	context.result = 0;
    timer_start(&clock);
    sample_input(sample_pt1, handle_line, &context);
	score_card_pt1(&context);
    time_end(&clock, "[PART1:SAMPLE]");
    printf("result: %ld\n", context.result);
	ASSERT_EQUAL(context.result, 13, "Part 1 sample answer did not match");

    context.winning_count = 10;
    context.selected_count = 25;
    context.card_count = 0;
    context.result = 0;
    timer_start(&clock);
    read_input("day4.txt", handle_line, handle_error, &context);
    score_card_pt1(&context);
    time_end(&clock, "[PART1:INPUT]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 23847, "Part 1 input answer did not match");

    context.winning_count = 5;
    context.selected_count = 8;
    context.card_count = 0;
    context.result = 0;
    timer_start(&clock);
    sample_input(sample_pt1, handle_line, (void *) &context);
    context.result = score_card_pt2(&context, 0, context.card_count);
    time_end(&clock, "[PART2:SAMPLE]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result,  30, "Part 2 sample answer did not match");

    context.winning_count = 10;
    context.selected_count = 25;
    context.card_count = 0;
    context.result = 0;
    timer_start(&clock);
    read_input("day4.txt", handle_line, handle_error, &context);
    context.result = score_card_pt2(&context, 0, context.card_count);
    time_end(&clock, "[PART2:INPUT]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result,  8570000, "Part 2 input answer did not match");

    free_expr(card);

    return 0;
}

