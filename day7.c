#include <stdio.h>
#include <stdlib.h>
#include "input.h"
#include "macros.h"
#include "timer.h"

typedef unsigned long value_t;

typedef struct {
    char cards[5];
    int bet;
    int score;
} hand_t;

#define MAX_HANDS 1000
typedef struct {
    value_t result;
    hand_t hands[MAX_HANDS];
    int num_hands;
    char wildcard;
} context_t;

/* No easy way to pass context into qsort, so global variable it is... */
const char *card_values = "AKQJT98765432";

int card_to_index(char c) {
    const char *ptr;
    if ((ptr = strchr(card_values, c)) != NULL) {
        return (int) (ptr - card_values);
    } else {
        return -1;
    }
}

typedef struct {
    int card;
    int count;
} pair_t;

int pair_compare(const void *a, const void *b) {
    pair_t* pair_a = (pair_t*) a;
    pair_t* pair_b = (pair_t*) b;
    return pair_b->count - pair_a->count;
}

int score_hand(hand_t* hand, char wildcard) {
    int i, j, score, wildcard_count = 0;
    pair_t counts[14];
    int wildcard_index = card_to_index(wildcard);

    for(i = 0; i < NELEM(counts); i++) {
        counts[i].card = i;
        counts[i].count = 0;
    }

    /* Count each type of card in the hand */
    for(j = 0; j < 5; j++) {
        counts[card_to_index(hand->cards[j])].count++;
    }

    qsort(counts, NELEM(counts), sizeof(pair_t), pair_compare);

    /* determine if we have any wildcards in hand */
    if (wildcard_index > 0) {
        for(i = 0; i < NELEM(counts)  && counts[i].count > 0; i++) {
            if (wildcard_index == counts[i].card) {
                wildcard_count = counts[i].count;
                if (wildcard_count == 5) {
                    return wildcard_count * wildcard_count * 10;
                }
                break;
            }
        }
    }

    /* Create a score for the hand */
    score = 0;
    for(i = 0; i < NELEM(counts)  && counts[i].count > 0; i++) {
        if (counts[i].card != wildcard_index) {
            if (wildcard_index > 0 && wildcard_count > 0) {
                score += (counts[i].count + wildcard_count) * (counts[i].count + wildcard_count) * 10;
                wildcard_count = 0;
            } else {
                score += counts[i].count * counts[i].count * 10;
            }
        }
    }

    return score;
}

void handle_input(const char *line, int line_num, void* ctx) {
    context_t* context = (context_t *) ctx;
    strncpy(context->hands[context->num_hands].cards, line, 5);
    context->hands[context->num_hands].bet = strtol(line + 6, NULL, 10);
    context->hands[context->num_hands].score = score_hand(&context->hands[context->num_hands], context->wildcard);
    context->num_hands++;
}

void handle_error(const char *msg, void* ctx) {
    ASSERT(0, msg);
}

int compare_hands(const void* a, const void* b) {
    hand_t *hand_a = (hand_t *) a;
    hand_t *hand_b = (hand_t *) b;
    int rv =  hand_a->score - hand_b->score, i = 0;
    while (rv == 0 && i < NELEM(hand_a->cards)) {
        rv = card_to_index(hand_b->cards[i]) - card_to_index(hand_a->cards[i]);
        i++;
    }
    return rv;
}

void rank_hands(context_t* context) {
    int i;
    qsort(context->hands, context->num_hands, sizeof(hand_t), compare_hands);
    for (i = 0; i < context->num_hands; i++) {
        context->result += (i + 1) * context->hands[i].bet;
    }
}


int main() {
    time_t clock;
    context_t context;
    const char *sample = "32T3K 765\n"
                         "T55J5 684\n"
                         "KK677 28\n"
                         "KTJJT 220\n"
                         "QQQJA 483";

    card_values = "AKQJT98765432";

    context.result = 0;
    context.num_hands = 0;
    context.wildcard = ' ';
    timer_start(&clock);
    sample_input(sample, handle_input, &context);
    rank_hands(&context);
    time_end(&clock, "[PART1:SAMPLE]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 6440, "Part 1 sample answer did not match");

    context.result = 0;
    context.num_hands = 0;
    context.wildcard = ' ';
    timer_start(&clock);
    read_input("day7.txt", handle_input, handle_error, &context);
    rank_hands(&context);
    time_end(&clock, "[PART1:INPUT]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 246795406, "Part 1 input answer did not match");

    /* Jack aka Joker is weakest */
    card_values = "AKQT98765432J";

    context.result = 0;
    context.num_hands = 0;
    context.wildcard = 'J';
    timer_start(&clock);
    sample_input(sample, handle_input, &context);
    rank_hands(&context);
    time_end(&clock, "[PART2:SAMPLE]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 5905, "Part 2 sample answer did not match");

    context.result = 0;
    context.num_hands = 0;
    context.wildcard = 'J';
    timer_start(&clock);
    read_input("day7.txt", handle_input, handle_error, &context);
    rank_hands(&context);
    time_end(&clock, "[PART2:INPUT]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 249356515, "Part 2 input answer did not match");
    return 0;
}