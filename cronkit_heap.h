#ifndef CRONKIT_HEAP_H
#define CRONKIT_HEAP_H

#include <stdlib.h>

typedef int cronkit_heap_compare_handler(const void *a, const void *b);
typedef int cronkit_heap_cleanup_handler(void *v);

#define cronkit_heap_size(h)  ((h)->idx)

struct cronkit_heap_t
{
    int cap;
    int idx;
    void **ptr;
    cronkit_heap_compare_handler *compare;
    cronkit_heap_cleanup_handler *cleanup;
};

#define cronkit_heap_init(h, c, cmp, cup) do {          \
    (h)->cap = (c);                                \
    (h)->idx = 0;                                  \
    (h)->ptr = malloc(sizeof(void *) * ((c) + 1)); \
    (h)->compare = (cmp);                           \
    (h)->cleanup = (cup);                           \
} while (0)

#define cronkit_heap_cleanup(heap) do {     \
    if ((heap)) {                             \
        if ((heap)->ptr) {                    \
            void *v;                        \
            for (;;) {                      \
                v = cronkit_heap_pop((heap)); \
                if (!v) {                   \
                    break;                  \
                }                           \
                if ((heap)->cleanup) {        \
                    (heap)->cleanup(v);       \
                }                           \
            }                               \
            free((heap)->ptr);                \
            (heap)->ptr = NULL;               \
        }                                   \
    }                                       \
} while (0)


struct cronkit_heap_t *cronkit_heap_alloc(int cap,
                                          cronkit_heap_compare_handler *cmp,
                                          cronkit_heap_cleanup_handler *cleanup);

void *cronkit_heap_peek(struct cronkit_heap_t *h);
void cronkit_heap_free(struct cronkit_heap_t *h);
void *cronkit_heap_put(struct cronkit_heap_t *h, void *data);
void *cronkit_heap_pop(struct cronkit_heap_t *h);

#endif