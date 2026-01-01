#include "hmap.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Hmap _hmap_create(size_t value_size) {
    struct Hmap hmap;
    for (int i = 0; i < HMAP_MAX_BUCKETS; i++) {
        struct hmap_BucketItem empty = {{0}, NULL, NULL, NULL};
        hmap.buckets[i] = empty;
    }
    hmap.value_size = value_size;
    return hmap;
}

void hmap_destroy(struct Hmap *self) {
    for (int i = 0; i < HMAP_MAX_BUCKETS; i++) {
        if (!self->buckets[i].value)
            continue;
        struct hmap_BucketItem *bck_item = &self->buckets[i];
        while (bck_item->next)
            bck_item = bck_item->next;
        while (bck_item->prev) {
            struct hmap_BucketItem *prev = bck_item->prev;
            free(bck_item->value);
            free(bck_item);
            bck_item = prev;
        }
        memset(bck_item, 0, sizeof(struct hmap_BucketItem));
    }
}

size_t hash_fnv1a(const char *s) {
    size_t hash = 2166136261u;
    while (*s) {
        hash ^= (unsigned char)*s++;
        hash *= 16777619u;
    }
    return hash;
}

static struct hmap_BucketItem *hmap_bucket_at(struct Hmap *self,
                                              const char *key) {
    if (strlen(key) > HMAP_MAX_KEY_LEN) {
        errno = EINVAL;
        return NULL;
    }
    size_t hash = hash_fnv1a(key);
    size_t idx = hash % HMAP_MAX_BUCKETS;
    struct hmap_BucketItem *bck_item = &self->buckets[idx];
    if (!bck_item->value) {
        errno = EINVAL;
        return NULL;
    }
    return bck_item;
}

static struct hmap_BucketItem *hmap_pair_at(struct Hmap *self,
                                            const char *key) {
    struct hmap_BucketItem *bck_item = hmap_bucket_at(self, key);
    if (!bck_item)
        return NULL;
    while (memcmp(bck_item->key, key, strlen(key)) && bck_item->next)
        bck_item = bck_item->next;
    if (!bck_item)
        return NULL;
    return bck_item;
}

void *hmap_at(struct Hmap *self, const char *key) {
    struct hmap_BucketItem *bck_item = hmap_pair_at(self, key);
    if (!bck_item)
        return NULL;
    return bck_item->value;
}

// returns 0 if success, -1 if key has already been reserved
int hmap_append(struct Hmap *self, const char *key, void *value) {
    if (hmap_at(self, key))
        return -1;
    size_t hash = hash_fnv1a(key);
    size_t idx = hash % HMAP_MAX_BUCKETS;
    struct hmap_BucketItem *bck_item = &self->buckets[idx];
    if (!bck_item->value) {
        strncpy(bck_item->key, key, HMAP_MAX_KEY_LEN);
        bck_item->value = calloc(1, self->value_size);
        memcpy(bck_item->value, value, self->value_size);
    } else {
        struct hmap_BucketItem *prev;
        while (bck_item->next) {
            prev = bck_item;
            bck_item = bck_item->next;
        }
        bck_item->next = calloc(1, sizeof(struct hmap_BucketItem));
        prev = bck_item;
        bck_item = bck_item->next;
        strncpy(bck_item->key, key, HMAP_MAX_KEY_LEN);
        bck_item->value = calloc(1, self->value_size);
        memcpy(bck_item->value, value, self->value_size);
        bck_item->next = NULL;
        bck_item->prev = prev;
    }
    return 0;
}

int hmap_pop(struct Hmap *self, const char *key) {
    struct hmap_BucketItem *bck_item = hmap_pair_at(self, key);
    if (!bck_item)
        return -1;
    struct hmap_BucketItem *prev = bck_item->prev;
    struct hmap_BucketItem *reconn_ptr = bck_item->next;
    free(bck_item);
    if (prev)
        prev->next = reconn_ptr;
    reconn_ptr->prev = prev;
    return 0;
}
