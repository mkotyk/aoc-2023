#include <stdio.h>
#include <stdlib.h>
#include "input.h"
#include "macros.h"
#include "timer.h"
#include "vector.h"

typedef unsigned long value_t;

typedef struct {
    value_t dst_start;
    value_t src_start;
    value_t length;
} offset_range_t;

typedef struct {
    char name[30];
    vector_t ranges;
} category_t;

typedef struct {
    value_t result;
    value_t seeds[20];
    int num_seeds;
    vector_t categories;
} context_t;

int parse_digits(const char *input, unsigned long* output) {
    const char *start = input;
    char *end;
    int count = 0;
    while(1) {
        output[count] = strtoul(start, &end, 10);
        count++;
        if (*end == '\0') break;
        start = end + 1;
    }
    return count;
}

void parse_range(const char *input, offset_range_t* range) {
    const char *start = input;
    char *end;
    range->dst_start = strtoul(start, &end, 10);
    start = end + 1;
    range->src_start = strtoul(start, &end, 10);
    start = end + 1;
    range->length = strtoul(start, &end, 10);
}

void handle_line(const char *line, int line_num, void* ctx) {
    context_t *context = (context_t *) ctx;
    const char *ptr;
    category_t* cat;
    offset_range_t  *range;
    if (strlen(line) == 0) return;
    if (strstr(line, "seeds:") == line) {
        context->num_seeds = parse_digits(line + 6, context->seeds);
    } else if ((ptr = strstr(line, "map:")) != NULL) {
        cat = malloc(sizeof(category_t));
        memset(cat, 0, sizeof(category_t));
        strncpy(cat->name, line, ptr - line);
        vec_insert_back(&context->categories, cat);
    } else {
        range = malloc(sizeof(offset_range_t));
        memset(range, 0, sizeof(offset_range_t));
        parse_range(line, range);
        vec_insert_back(
            &((category_t *)context->categories.last->data)->ranges,
            range
        );
    }
}

value_t map(offset_range_t* r, value_t i) {
    return i - r->src_start + r->dst_start;
}

void translate_range(vector_t* ranges, vector_t* split_ranges) {
    vec_node_t* vp, *sr_vp;
    offset_range_t* r, *sr, *n;
    sr_vp = split_ranges->first;
    while(sr_vp != NULL) {
        sr = (offset_range_t *) sr_vp->data;
        vp = ranges->first;
        while (vp != NULL) {
            r = (offset_range_t *) vp->data;
            if (r->src_start <= sr->src_start && r->src_start + r->length >= sr->src_start) {
                // Start is in range
                if (r->src_start <= sr->dst_start && r->src_start + r->length >= sr->dst_start) {
                    // Both are in range, no split, just translate
                    sr->src_start = map(r, sr->src_start);
                    sr->dst_start = map(r, sr->dst_start);
                    break;
                } else {
                    // Start is in, but dest is out - Split
                    n = malloc(sizeof(offset_range_t));
                    n->dst_start = sr->dst_start;
                    sr->dst_start = r->src_start + r->length;
                    n->src_start = sr->dst_start + 1;
                    n->length = 0;
                    vec_insert_back(split_ranges, n);
                    continue;
                }
            } else {
                if (r->src_start <= sr->dst_start && r->src_start + r->length >= sr->dst_start) {
                    // Only dest is in range - Split
                    n = malloc(sizeof(offset_range_t));
                    n->src_start = sr->src_start;
                    sr->src_start = r->src_start;
                    n->dst_start = sr->src_start - 1;
                    n->length = 0;
                    vec_insert_back(split_ranges, n);
                    continue;
                }
            }
            vp = vp->next;
        }
        sr_vp = sr_vp->next;
    }
}

void do_translate_range(vector_t* categories, vector_t* split_ranges) {
    vec_node_t* vp;
    vp = categories->first;
    while(vp != NULL) {
        translate_range( &((category_t*) vp->data)->ranges, split_ranges);
        vp = vp->next;
    }
}

void handle_error(const char *msg, void* ctx) {
    ASSERT(0, msg);
}

int free_range(vector_t* vector, size_t index, void* data, void *ctx) {
    offset_range_t* range = (offset_range_t*) data;
    free(range);
    return 1;
}

int free_category(vector_t* vector, size_t index, void* data, void *ctx) {
    category_t* cat = (category_t *) data;
    vec_iterate(&cat->ranges, &free_range, NULL);
    free(cat);
    return 1;
}

void reset_context(context_t* context) {
    context->result = 0;
    context->num_seeds = 0;
    vec_iterate(&context->categories, &free_category, NULL);
    memset(&context->categories, 0, sizeof(vector_t));
}

void find_lowest_location(context_t* context) {
    int i;
    context->result = 0xFFFFFFFF;
    vector_t split_ranges;
    vec_node_t * vp;
    offset_range_t *in, *r;

    memset(&split_ranges, 0, sizeof(vector_t ));
    for(i = 0; i < context->num_seeds; i++) {
        in = malloc(sizeof(offset_range_t));
        in->src_start = in->dst_start = context->seeds[i];
        in->length = 1;
        vec_insert_back(&split_ranges, in);
    }
    do_translate_range(&context->categories, &split_ranges);

    vp = split_ranges.first;
    while(vp != NULL) {
        r = (offset_range_t *) vp->data;
        if (r->src_start < context->result) context->result = r->src_start;
        vp = vp->next;
    }

    vec_iterate(&split_ranges, &free_range, NULL);
    vec_free(&split_ranges);
}

void find_lowest_location_pt2(context_t* context) {
    int i;
    offset_range_t *in, *r;
    vec_node_t * vp;
    vector_t split_ranges;
    memset(&split_ranges, 0, sizeof(vector_t ));
    context->result = 0xFFFFFFFF;
    for(i = 0; i < context->num_seeds; i+=2) {
        in = malloc(sizeof(offset_range_t));
        in->src_start = context->seeds[i];
        in->length = context->seeds[i + 1];
        in->dst_start = in->src_start + in->length;
        vec_insert_back(&split_ranges, in);
    }
    do_translate_range(&context->categories, &split_ranges);

    vp = split_ranges.first;
    while(vp != NULL) {
        r = (offset_range_t *) vp->data;
        if (r->src_start < context->result) context->result = r->src_start;
        if (r->dst_start < context->result) context->result = r->dst_start;
        vp = vp->next;
    }
    vec_iterate(&split_ranges, &free_range, NULL);
    vec_free(&split_ranges);
}

int main() {
    time_t clock;
    context_t context;
    const char *sample_pt1 = "seeds: 79 14 55 13\n"
                             "\n"
                             "seed-to-soil map:\n"
                             "50 98 2\n"
                             "52 50 48\n"
                             "\n"
                             "soil-to-fertilizer map:\n"
                             "0 15 37\n"
                             "37 52 2\n"
                             "39 0 15\n"
                             "\n"
                             "fertilizer-to-water map:\n"
                             "49 53 8\n"
                             "0 11 42\n"
                             "42 0 7\n"
                             "57 7 4\n"
                             "\n"
                             "water-to-light map:\n"
                             "88 18 7\n"
                             "18 25 70\n"
                             "\n"
                             "light-to-temperature map:\n"
                             "45 77 23\n"
                             "81 45 19\n"
                             "68 64 13\n"
                             "\n"
                             "temperature-to-humidity map:\n"
                             "0 69 1\n"
                             "1 0 69\n"
                             "\n"
                             "humidity-to-location map:\n"
                             "60 56 37\n"
                             "56 93 4";
    memset(&context.categories, 0, sizeof(vector_t));

    reset_context(&context);
    timer_start(&clock);
    sample_input(sample_pt1, handle_line, &context);
    find_lowest_location(&context);
    time_end(&clock, "[PART1:SAMPLE]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 35, "Part 1 sample answer did not match");


    reset_context(&context);
    timer_start(&clock);
    read_input("day5.txt", handle_line, handle_error, &context);
    find_lowest_location(&context);
    time_end(&clock, "[PART1:INPUT]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 382895070, "Part 1 input answer did not match");

    reset_context(&context);
    timer_start(&clock);
    sample_input(sample_pt1, handle_line, &context);
    find_lowest_location_pt2(&context);
    time_end(&clock, "[PART2:SAMPLE]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result,  46, "Part 2 sample answer did not match");

    reset_context(&context);
    timer_start(&clock);
    read_input("day5.txt", handle_line, handle_error, &context);
    find_lowest_location_pt2(&context);
    time_end(&clock, "[PART2:INPUT]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result,  17729182, "Part 2 input answer did not match");
    reset_context(&context);

    return 0;
}

