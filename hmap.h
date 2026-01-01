#ifndef HMAP_H
#define HMAP_H

#include "vector.h"
#include <stdio.h>

#define HMAP_MAX_BUCKETS 100
#define HMAP_MAX_KEY_LEN 64

struct hmap_BucketItem {
    char key[HMAP_MAX_KEY_LEN];
    void *value;
    struct hmap_BucketItem *next;
    struct hmap_BucketItem *prev;
};

struct Hmap {
    struct hmap_BucketItem buckets[HMAP_MAX_BUCKETS];
    size_t value_size;
};

#define hmap_create(type) _hmap_create(sizeof(type))
struct Hmap _hmap_create(size_t value_size);
void hmap_destroy(struct Hmap *self);
void *hmap_at(struct Hmap *self, const char *key);
int hmap_append(struct Hmap *self, const char *key, void *value);
int hmap_pop(struct Hmap *self, const char *key);

#endif
