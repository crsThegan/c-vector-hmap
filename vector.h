#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>

#define VECTOR_CAP_FACTOR 2
#define VECTOR_INIT_CAP 10

struct Vector {
    void *data;
    size_t len;
    size_t cap;
    size_t el_size;
};

#define vector_create(type) _vector_create(sizeof(type))
struct Vector _vector_create(size_t el_size);
void vector_destroy(struct Vector *self);
void *vector_at(struct Vector *self, size_t idx);
int vector_append(struct Vector *self, size_t idx, void *value);
int vector_pop(struct Vector *self, size_t idx);
size_t vector_find(struct Vector *self, void *value);

#endif
