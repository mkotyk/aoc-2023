#ifndef AOC_2023_PARSER_H
#define AOC_2023_PARSER_H
typedef struct {
    const char *start;
    const char *end;
} range_t;

typedef struct {
    const char *start;
    const char *end;
    const char *pos;
    range_t *captures;
    int max_captures;
    int capture_count;
} parse_context_t;

struct expr_base;
typedef struct expr_base* expr_t;

/* Expression builders */
expr_t literal(const char *literal);
expr_t all(expr_t first, ...);
expr_t capture(expr_t inner);
expr_t repeat(int min, int max, expr_t inner);
expr_t or(expr_t a, expr_t b);
expr_t range(char min, char max);
expr_t optional(expr_t inner);
expr_t some(expr_t inner);

expr_t whitespace();
expr_t digit();
expr_t number();

expr_t start();
expr_t end();

parse_context_t* init_parse_context(const char *string, int max_captures);
void free_parse_context(parse_context_t* parse_context);
int evaluate(expr_t root, parse_context_t* parse_context);

void free_expr(expr_t expr);

#endif