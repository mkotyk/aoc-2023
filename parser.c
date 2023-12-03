#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef struct {
    const char *start;
    const char *end;
} range_t;

const int MAX_MATCHES = 50;

typedef struct {
    const char *start;
    const char *end;
    const char *pos;
    range_t matches[MAX_MATCHES];
    int match_count;
} parse_context_t;

struct expr_base;
typedef struct expr_base* expr_t;

typedef int (*evaluate_cb)(expr_t, parse_context_t *);
typedef void (*free_cb)(expr_t);

struct expr_base {
    evaluate_cb eval;
    free_cb free;
};

struct expr_capture {
    struct expr_base base;
    expr_t inner;
};

struct expr_repeat {
    struct expr_base base;
    int min;
    int max;
    int greedy;
    expr_t inner;
};

struct expr_literal {
    struct expr_base base;
    const char *literal;
};

struct expr_range {
    struct expr_base base;
    char start;
    char end;
};

struct expr_all {
    struct expr_base base;
    expr_t* children;
    int num_children;
};

struct expr_or {
    struct expr_base base;
    expr_t a;
    expr_t b;
};

struct expr_char_class {
    struct expr_base base;
    char min;
    char max;
};

int evaluate_literal(expr_t me, parse_context_t *ctx) {
    struct expr_literal* expr = (struct expr_literal *) me;
    return 0;
}

void free_literal(expr_t me) {
    struct expr_literal* expr = (struct expr_literal *) me;
    free(expr);
}

expr_t literal(const char *literal) {
    struct expr_literal* expr = malloc(sizeof(struct expr_literal));
    if (expr != NULL) {
        expr->base.eval = evaluate_literal;
        expr->base.free = free_literal;
        expr->literal = literal;
    }
    return (expr_t) expr;
}

int evaluate_all(expr_t me, parse_context_t *ctx) {
    struct expr_all *expr = (struct expr_all *) me;
    int i;
    int result = 1;
    for (i = 0; i < expr->num_children && result; i++) {
        result &= expr->children[i]->eval((struct expr_base *) &expr->children[i], ctx);
    }
    return result;
}

void free_all(expr_t me) {
    struct expr_all *expr = (struct expr_all *) me;
    int i;
    for(i = 0; i < expr->num_children; i++) {
        expr->children[i]->free(expr->children[i]);
    }
    free(expr->children);
    free(expr);
}

expr_t all_ex(expr_t* children, int num_children) {
    struct expr_all *expr = malloc(sizeof(struct expr_all));
    if (expr != NULL) {
        expr->base.eval = evaluate_all;
        expr->base.free = free_all;
        expr->children = children;
        expr->num_children = num_children;
    }
    return (expr_t) expr;
}

expr_t all(expr_t first, ...) {
    int count = 1;
    expr_t* items;
    va_list args;

    /* TODO: Args are all the same size, can we just calc */
    va_start(args, first);
    count++;
    va_end(args);
    items = malloc(sizeof(expr_t) * count);
    if (items != NULL) {
        count = 0;
        items[count++] = first;
        va_start(args, first);
        items[count++] = va_arg(args, expr_t);
        va_end(args);
        return all_ex(items, count);
    } else {
        return NULL;
    }
}

int evaluate_capture(expr_t me, parse_context_t *ctx) {
    struct expr_capture *expr = (struct expr_capture *) me;
    int result;
    range_t match;
    match.start = ctx->pos;
    result = expr->inner->eval(expr->inner, ctx);
    if (result) {
        match.end = ctx->pos;
        ctx->matches[ctx->match_count] = match;
        ctx->match_count++;
    }
    return result;
}

void free_capture(expr_t me) {
    struct expr_capture* expr = (struct expr_capture *) me;
    expr->inner->free(expr->inner);
    free(me);
}

expr_t capture(expr_t inner) {
    struct expr_capture* expr = malloc(sizeof(struct expr_capture));
    if (expr != NULL) {
        expr->base.eval = evaluate_capture;
        expr->base.free = free_capture;
        expr->inner = inner;
    }
    return (expr_t) expr;
}

int evaluate_repeat(expr_t me, parse_context_t *ctx) {
    struct expr_repeat *expr = (struct expr_repeat *) me;
    int i = 0;
    while (i != expr->max) {
        if (!expr->inner->eval(expr->inner, ctx)) break;
        if (!expr->greedy && i >= expr->min) break;
        i++;
    }
    return i >= expr->min && i <= expr->max;
}

void free_repeat(expr_t me) {
    struct expr_repeat* expr = (struct expr_repeat *) me;
    expr->inner->free(expr->inner);
    free(me);
}

expr_t repeat_ex(int min, int max, int greedy, expr_t inner) {
    struct expr_repeat* expr = malloc(sizeof(struct expr_repeat));
    if (expr != NULL) {
        expr->base.eval = evaluate_repeat;
        expr->base.free = free_repeat;
        expr->min = min;
        expr->max = max;
        expr->greedy = greedy;
        expr->inner = inner;
    }
    return (expr_t) expr;
}

expr_t repeat(int min, int max, expr_t inner) {
    return repeat_ex(min, max, 1, inner);
}

int evaluate_or(expr_t me, parse_context_t* ctx) {
    return 0;
}

void free_or(expr_t me) {
    struct expr_or* expr = (struct expr_or *) me;
    expr->a->free(expr->a);
    expr->b->free(expr->b);
    free(expr);
}

expr_t or(expr_t a, expr_t b) {
    struct expr_or* expr = malloc(sizeof(struct expr_or));
    if (expr != NULL) {
        expr->base.eval = evaluate_or;
        expr->base.free = free_or;
        expr->a = a;
        expr->b = b;
    }
    return (expr_t) expr;
}

int evaluate_char_class(expr_t me, parse_context_t* ctx) {
    struct expr_char_class* expr = (struct expr_char_class *) me;
    int valid = (*ctx->pos) <= expr->min && (*ctx->pos) >= expr->max;
    if (valid) ctx->pos++;
    return valid;
}

void free_char_class(expr_t me) {
    struct expr_char_class* expr = (struct expr_char_class *) me;
    free(expr);
}

expr_t char_class(char min, char max) {
    struct expr_char_class* expr = (struct expr_char_class *) malloc(sizeof(struct expr_char_class));
    if (expr != NULL) {
        expr->base.eval = evaluate_char_class;
        expr->base.free = free_char_class;
        expr->min = min;
        expr->max = max;
    }
    return (expr_t) expr;
}

parse_context_t* init_context(const char *string) {
    parse_context_t* ctx = malloc(sizeof(parse_context_t));
    if (ctx != NULL) {
        ctx->pos = ctx->start = string;
        ctx->end = string + strlen(string);
        ctx->match_count = 0;
    }
    return ctx;
}

expr_t whitespace() {
    return or(literal(" "), or(literal("\t"), or(literal("\n"), literal("\r"))));
}

expr_t digit() {
    return char_class('0','9');
}

void test() {
    parse_context_t* ctx = init_context("Game 12: 3 blue, 4 red; 5 red, 6 green");

    expr_t root = all(
            literal("Game"),
            whitespace(),
            capture(repeat(1,2, digit())),
            literal(":"),
            repeat(1,99,
                   all(whitespace())
                   )
            );

    root->eval(root, ctx);

    root->free(root);
    free(ctx);
}
