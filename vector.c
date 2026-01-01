#include "vector.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

struct Vector _vector_create(size_t el_size) {
    struct Vector vec;
    vec.el_size = el_size;
    vec.cap = VECTOR_INIT_CAP;
    vec.data = malloc(vec.cap * vec.el_size);
    memset(vec.data, 0, vec.cap * vec.el_size);
    vec.len = 0;
    return vec;
}

void vector_destroy(struct Vector *self) {
    free(self->data);
    memset(self, 0, sizeof(struct Vector));
}

void *vector_at(struct Vector *self, size_t idx) {
    if (idx < 0 || idx >= self->len) {
        errno = EINVAL;
        return NULL;
    }
    return self->data + idx * self->el_size;
}

int vector_append(struct Vector *self, size_t idx, void *value) {
    if (idx < 0 || idx > self->len)
        return EINVAL;
    if (++self->len > self->cap) {
        self->cap *= VECTOR_CAP_FACTOR;
        void *tmp = realloc(self->data, self->cap * self->el_size);
        if (!tmp)
            return errno;
        self->data = tmp;
    }
    memmove(self->data + (idx + 1) * self->el_size,
            self->data + idx * self->el_size,
            (self->len - idx) * self->el_size);
    memcpy(self->data + idx * self->el_size, value, self->el_size);
    return 0;
}

int vector_pop(struct Vector *self, size_t idx) {
    memmove(self->data + idx * self->el_size,
            self->data + (idx + 1) * self->el_size,
            (self->len - idx - 1) * self->el_size);
    if (--self->len <= self->cap / VECTOR_CAP_FACTOR) {
        self->cap /= VECTOR_CAP_FACTOR;
        void *tmp = realloc(self->data, self->cap * self->el_size);
        if (!tmp)
            return errno;
        self->data = tmp;
    }
    return 0;
}

size_t vector_find(struct Vector *self, void *value) {
    for (size_t i = 0; i < self->len; i++)
        if (!memcmp(vector_at(self, i), value, self->el_size))
            return i;
    errno = EINVAL;
    return self->len;
}
