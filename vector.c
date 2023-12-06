#include "vector.h"

void vec_free(vector_t* vector) {
    vec_node_t* ptr = vector->first, *save;
    while(ptr != NULL) {
        save = ptr;
        ptr = ptr->next;
        free(save);
    }
}

int vec_insert_back(vector_t* vector, void* data) {
    vec_node_t* new_node = malloc(sizeof(vec_node_t));
    if (new_node != NULL) {
        new_node->data = data;
        new_node->next = NULL;
        if (vector->last != NULL) {
            new_node->prev = vector->last;
            vector->last->next = new_node;
        }
        vector->last = new_node;
        if (vector->first == NULL) vector->first = new_node;
        vector->size++;
        return 1;
    }
    return 0;
}

int vec_insert_front(vector_t* vector, void* data) {
    vec_node_t* new_node = malloc(sizeof(vec_node_t));
    if (new_node != NULL) {
        new_node->data = data;
        new_node->prev = NULL;
        if (vector->first != NULL) {
            new_node->next = vector->first;
            vector->first->prev = new_node;
        }
        vector->first = new_node;
        if (vector->last == NULL) vector->last = new_node;
        vector->size++;
        return 1;
    }
    return 0;
}

vec_node_t* vec_seek(vector_t* vector, size_t index) {
    vec_node_t *ptr = vector->first;
    size_t i = 0;
    while (ptr != NULL) {
        if (i == index) break;
        ptr = ptr->next;
        i++;
    }
    return ptr;
}

int vec_element_at(vector_t* vector, size_t index, void** result) {
    vec_node_t* ptr = vec_seek(vector, index);
    if (result != NULL && ptr != NULL) {
        *result = ptr->data;
        return 1;
    }
    return 0;
}

size_t vec_iterate(vector_t* vector, vec_item_cb on_each_item, void* ctx) {
    vec_node_t* ptr = vector->first;
    size_t index = 0;
    while(ptr != NULL) {
        if (!on_each_item(vector, index, ptr->data, ctx)) break;
        ptr = ptr->next;
        index++;
    }
    return index;
}

size_t vec_iterate_range(vector_t* vector, size_t start, size_t end, vec_item_cb on_each_item, void* ctx) {
    vec_node_t* ptr = vec_seek(vector, start);
    size_t index = start;
    while(ptr != NULL) {
        if (!on_each_item(vector, index, ptr->data, ctx)) break;
        if (index == end) break;
        ptr = ptr->next;
        index++;
    }
    return index;
}
