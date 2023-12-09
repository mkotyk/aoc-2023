#include <stdio.h>
#include <stdlib.h>
#include "input.h"
#include "macros.h"
#include "timer.h"

#define MAX_NODE_NAME 4
#define MAX_NODES 1000
#define MAX_PATH_LEN 300

typedef unsigned long value_t;
struct node {
    char name[MAX_NODE_NAME];
    char left[MAX_NODE_NAME];
    struct node* ll;
    char right[MAX_NODE_NAME];
    struct node* rl;
};
typedef struct node node_t;

typedef struct {
    value_t result;
    char path[MAX_PATH_LEN];
    node_t nodes[MAX_NODES];
    int max_node;
    node_t* start;
    node_t* end;
} context_t;

void handle_line(const char *line, int line_num, void* ctx) {
    context_t* context = (context_t *) ctx;

    if (strlen(line) == 0) return;
    if (line_num == 1) {
        ASSERT(strlen(line) < NELEM(context->path), "Path length too long, increase allocation");
        strcpy(context->path, line);
    } else {
        memset(&context->nodes[context->max_node], 0, sizeof(struct node));
        memcpy(&context->nodes[context->max_node].name, line, 3);
        memcpy(&context->nodes[context->max_node].left, line + 7, 3);
        memcpy(&context->nodes[context->max_node].right, line + 12, 3);
        context->max_node++;
        ASSERT(context->max_node < MAX_NODES, "Too many nodes, increase allocation");
    }
}

void handle_error(const char *msg, void* ctx) {
    ASSERT(0, msg);
}

void link_nodes(context_t* context) {
    int i,j;
    node_t* n, *m;
    context->start = NULL;

    for(i = 0; i < context->max_node; i++) {
        n = &context->nodes[i];
        if (context->start == NULL && strcmp(n->name, "AAA") == 0)
            context->start = n;

        if (n->ll && n->rl) continue;
        for (j = 0; j < context->max_node; j++) {
            m = &context->nodes[j];
            if (strcmp(n->left, m->name) == 0) {
                n->ll = m;
            }
            if (strcmp(n->right, m->name) == 0) {
                n->rl = m;
            }
        }
    }
}

typedef int (*predicate_cb)(node_t*);

int pred_is_zzz(node_t* n) {
    return strcmp(n->name, "ZZZ") == 0;
}

int pred_ends_in_z(node_t* n) {
    return n->name[2] == 'Z';
}

value_t walk_path(context_t* context, predicate_cb p) {
    const char* path_ptr = context->path;
    node_t* node = context->start;
    value_t steps = 0;

    while(!p(node)) {
        switch(*path_ptr) {
            case 'L':
                node = node->ll;
                break;
            case 'R':
                node = node->rl;
                break;
            case '\0':
                path_ptr = context->path;
                printf("l");
                continue;
        }
        path_ptr++;
        steps++;
    }
    printf("\n");

    return steps;
}

value_t gcd (value_t a, value_t b) {
    value_t temp;
    while (b != 0) {
        temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

value_t lcm( value_t a, value_t b ) {
    return a * b / gcd (a, b);
}

value_t crt (value_t* div, int k) {
    int i;
    value_t add = div[0], result = div[0];
    for (i = 1 ;i < k; add = lcm(add, div[i++])) {
        for (; result % div[i] != 0 ;result += add );
    }
    return result;
}


void calc_steps(context_t* context) {
    value_t num[20];
    int i, j = 0;
    for(i = 0; i< context->max_node; i++) {
        if (context->nodes[i].name[2] == 'A') {
            context->start = &context->nodes[i];
            num[j] = walk_path(context, pred_ends_in_z);
            j++;
        }
    }
    context->result = crt(num, j);
}

int main() {
    time_t clock;
    context_t context;
    const char *sample = "LLR\n"
                         "\n"
                         "AAA = (BBB, BBB)\n"
                         "BBB = (AAA, ZZZ)\n"
                         "ZZZ = (ZZZ, ZZZ)";

    const char *sample_pt2 = "LR\n"
                             "\n"
                             "11A = (11B, XXX)\n"
                             "11B = (XXX, 11Z)\n"
                             "11Z = (11B, XXX)\n"
                             "22A = (22B, XXX)\n"
                             "22B = (22C, 22C)\n"
                             "22C = (22Z, 22Z)\n"
                             "22Z = (22B, 22B)\n"
                             "XXX = (XXX, XXX)";

    context.result = 0;
    context.max_node = 0;
    timer_start(&clock);
    sample_input(sample, handle_line, &context);
    link_nodes(&context);
    context.result = walk_path(&context, pred_is_zzz);
    time_end(&clock, "[PART1:SAMPLE]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 6, "Part 1 sample answer did not match");

    context.result = 0;
    context.max_node = 0;
    timer_start(&clock);
    read_input("day8.txt", handle_line, handle_error, &context);
    link_nodes(&context);
    context.result = walk_path(&context, pred_is_zzz);
    time_end(&clock, "[PART1:INPUT]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result, 21409, "Part 1 input answer did not match");

    context.result = 0;
    context.max_node = 0;
    timer_start(&clock);
    sample_input(sample_pt2, handle_line, (void *) &context);
    time_end(&clock, "[PART2:SAMPLE]");
    link_nodes(&context);
    calc_steps(&context);
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result,  6, "Part 2 sample answer did not match");

    context.result = 0;
    context.max_node = 0;
    timer_start(&clock);
    read_input("day8.txt", handle_line, handle_error, &context);
    link_nodes(&context);
    calc_steps(&context);
    time_end(&clock, "[PART2:INPUT]");
    printf("result: %ld\n", context.result);
    ASSERT_EQUAL(context.result,  21165830176709, "Part 2 input answer did not match");

    return 0;
}

