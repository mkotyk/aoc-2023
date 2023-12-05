#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "parser.h"


/**
 * literal:  Match a literal string
 */
struct expr_literal {
    struct expr_base base;
    const char *literal;
};

int evaluate_literal(expr_t me, parse_context_t *ctx) {
    struct expr_literal* expr = (struct expr_literal *) me;
    int valid = 1, index, length;
    length = strlen(expr->literal);

    /* Would exceed input length */
    if (ctx->pos + length > ctx->end) return 0;

    for(index = 0; index < length && valid; index++) {
        valid &= expr->literal[index] == ctx->pos[index];
    }
    if (valid) {
        ctx->pos += length;
    }
    return valid;
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

/**
 * all:  Require all child elements to match in order
 */
struct expr_all {
    struct expr_base base;
    expr_t* children;
    int num_children;
};

int evaluate_all(expr_t me, parse_context_t *ctx) {
    struct expr_all *expr = (struct expr_all *) me;
    int i;
    int valid = 1;
    const char *save_pos = ctx->pos;
    for (i = 0; i < expr->num_children && valid; i++) {
        valid &= expr->children[i]->eval(expr->children[i], ctx);
    }
    if (!valid) {
        ctx->pos = save_pos;
    }
    return valid;
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
    expr_t* items;
    va_list args;
    int index, count = 1;

    /* Count up number of args until NULL */
    va_start(args, first);
    while (va_arg(args, expr_t) != NULL) count++;
    va_end(args);

    if ((items = malloc(sizeof(expr_t) * count)) != NULL) {
        items[0] = first;
        va_start(args, first);
        for (index = 1; index < count; index++) {
            items[index] = va_arg(args, expr_t);
        }
        va_end(args);
        return all_ex(items, count);
    } else {
        return NULL;
    }
}

/**
 * capture:  Mark the matching range of the input into the capture list
 */
struct expr_capture {
    struct expr_base base;
    expr_t inner;
};

int evaluate_capture(expr_t me, parse_context_t *ctx) {
    struct expr_capture *expr = (struct expr_capture *) me;
    int valid;
    range_t match;
    match.start = ctx->pos;
    valid = expr->inner->eval(expr->inner, ctx);
    if (valid) {
        match.end = ctx->pos;
        if (ctx->captures != NULL && ctx->capture_count < ctx->max_captures) {
            ctx->captures[ctx->capture_count] = match;
            ctx->capture_count++;
        }
    }
    return valid;
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

/**
 * repeat:  Require inner expression to match the number of times specified in the range
 */
struct expr_repeat {
    struct expr_base base;
    int min;
    int max;
    int greedy;
    expr_t inner;
};

int evaluate_repeat(expr_t me, parse_context_t *ctx) {
    struct expr_repeat *expr = (struct expr_repeat *) me;
    int i = 0, valid;
    const char *save_pos = ctx->pos;
    while (i != expr->max) {
        if (!expr->inner->eval(expr->inner, ctx)) break;
        if (!expr->greedy && i >= expr->min) break;
        i++;
    }
    valid = i >= expr->min && i <= expr->max;
    if (!valid) {
        ctx->pos = save_pos;
    }
    return valid;
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

/**
 * or:  Require either expression a or expression b to match
 */
struct expr_or {
    struct expr_base base;
    expr_t a;
    expr_t b;
};

int evaluate_or(expr_t me, parse_context_t* ctx) {
    struct expr_or* expr = (struct expr_or *) me;
    const char *save_pos = ctx->pos;
    int valid;
    valid = expr->a->eval(expr->a, ctx);
    if (!valid) {
        ctx->pos = save_pos;
        valid = expr->b->eval(expr->b, ctx);
        if (!valid) {
            ctx->pos = save_pos;
        }
    }

    return valid;
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

/**
 * Range:  Require a single character to match within the min-max character range
 */
struct expr_range {
    struct expr_base base;
    char min;
    char max;
};

int evaluate_char_class(expr_t me, parse_context_t* ctx) {
    struct expr_range* expr = (struct expr_range *) me;
    int valid = (*ctx->pos) >= expr->min && (*ctx->pos) <= expr->max;
    if (valid) ctx->pos++;
    return valid;
}

void free_char_class(expr_t me) {
    struct expr_range* expr = (struct expr_range *) me;
    free(expr);
}

expr_t range(char min, char max) {
    struct expr_range* expr = (struct expr_range *) malloc(sizeof(struct expr_range));
    if (expr != NULL) {
        expr->base.eval = evaluate_char_class;
        expr->base.free = free_char_class;
        expr->min = min;
        expr->max = max;
    }
    return (expr_t) expr;
}

/*******/

parse_context_t* init_parse_context(const char *string, int max_captures) {
    parse_context_t* ctx = malloc(sizeof(parse_context_t));
    if (ctx != NULL) {
        ctx->pos = ctx->start = string;
        ctx->end = string + strlen(string);
        ctx->max_captures = max_captures;
        ctx->captures = malloc(sizeof(range_t) * max_captures);
        ctx->capture_count = 0;
    }
    return ctx;
}

void free_parse_context(parse_context_t* parse_context) {
	if (parse_context->captures != NULL) {
		free(parse_context->captures);
	}
    free(parse_context);
}

int evaluate(expr_t root, parse_context_t* parse_context)
{
    return root->eval(root, parse_context);
}

expr_t whitespace() {
    return or(literal(" "), or(literal("\t"), or(literal("\n"), literal("\r"))));
}

expr_t digit() {
    return range('0','9');
}

expr_t number() {
    return repeat(1,99, or(digit(), literal(".")));
}

expr_t optional(expr_t inner) {
    return repeat(0,1, inner);
}

expr_t some(expr_t inner) {
    return repeat(1,32767, inner);
}

/**
 * Positional validators
 */
struct expr_position {
    struct expr_base base;
    int position;
};

int evaluate_position(expr_t me, parse_context_t* ctx) {
    struct expr_position* expr = (struct expr_position *) me;
    int valid = 0;
    switch(expr->position) {
        case -1:
            valid = ctx->pos == ctx->end;
            break;
        default:
            valid = ctx->pos == ctx->start + expr->position;
    }
    return valid;
}

void free_position(expr_t me) {
    struct expr_position* expr = (struct expr_position *) me;
    free(expr);
}

expr_t position(int index) {
    struct expr_position* expr = (struct expr_position *) malloc(sizeof(struct expr_position));
    if (expr != NULL) {
        expr->base.eval = evaluate_position;
        expr->base.free = free_position;
        expr->position = index;
    }
    return (expr_t) expr;
}

expr_t start() {
    return position(0);
}

expr_t end() {
    return position(-1);
}

void free_expr(expr_t expr) {
    expr->free(expr);
}

