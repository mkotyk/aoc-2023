#ifndef AOC_2023_VECTOR_H
#define AOC_2023_VECTOR_H

#include <stdlib.h>

struct vec_node;
typedef struct vec_node vec_node_t;
struct vec_node {
    vec_node_t* next;
    vec_node_t* prev;
    void* data;
};

typedef struct {
    vec_node_t* first;
    vec_node_t* last;
    size_t size;
} vector_t;

typedef int (*vec_item_cb)(vector_t*, size_t index, void* data, void* ctx);

void vec_free(vector_t* vector);
int vec_insert_back(vector_t* vector, void* data);
int vec_insert_front(vector_t* vector, void* data);
int vec_element_at(vector_t* vector, size_t index, void** result);
size_t vec_iterate(vector_t* vector, vec_item_cb on_each_item, void* ctx);
size_t vec_iterate_range(vector_t*, size_t start, size_t end, vec_item_cb on_each_item, void* ctx);

#endif
