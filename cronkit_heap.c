#include "cronkit_heap.h"


static void cronkit_heap_swap(void **ptr, int i, int j) {
    void *t = ptr[i];
    ptr[i] = ptr[j];
    ptr[j] = t;
}

static void cronkit_heap_swim(struct cronkit_heap_t *h, int i) {
    while (i > 1 && h->compare(*(h->ptr + (i / 2)), *(h->ptr + i))) {
        cronkit_heap_swap(h->ptr, i / 2, i);
        i = i / 2;
    }
}

static void cronkit_heap_sink(struct cronkit_heap_t *h, int i) {
    int pos;
    while (2 * i <= h->idx) {
        pos = 2 * i;
        if (pos < h->idx && (h->compare(*(h->ptr + pos), *(h->ptr + (pos + 1))))) {
            pos++;
        }
        if (!h->compare(*(h->ptr + i), *(h->ptr + pos))) {
            break;
        }
        cronkit_heap_swap(h->ptr, i, pos);
        i = pos;
    }
}

struct cronkit_heap_t *cronkit_heap_alloc(int cap,
                                          cronkit_heap_compare_handler *cmp,
                                          cronkit_heap_cleanup_handler *cleanup) {
    struct cronkit_heap_t *heap;
    heap = malloc(sizeof(struct cronkit_heap_t));
    if (heap == NULL) {
        return NULL;
    }
    heap->ptr = malloc(sizeof(void *) * (cap + 1));
    if (heap->ptr == NULL) {
        free(heap);
        return NULL;
    }
    heap->compare = cmp;
    heap->cleanup = cleanup;
    heap->cap = cap;
    heap->idx = 0;
    return heap;
}

void *cronkit_heap_put(struct cronkit_heap_t *h, void *data) {
    h->ptr[++h->idx] = data;
    cronkit_heap_swim(h, h->idx);
    return cronkit_heap_peek(h);
}

void *cronkit_heap_pop(struct cronkit_heap_t *h) {
    if (h->idx <= 0) {
        return NULL;
    }
    void *v = h->ptr[1];
    cronkit_heap_swap(h->ptr, 1, h->idx--);
    h->ptr[h->idx + 1] = NULL;
    cronkit_heap_sink(h, 1);
    return v;
}

void *cronkit_heap_peek(struct cronkit_heap_t *h) {
    if (h->idx <= 0) {
        return NULL;
    }
    return h->ptr[1];
}

void cronkit_heap_free(struct cronkit_heap_t *h) {
    if (h) {
        cronkit_heap_cleanup(h);
        free(h);
        h = NULL;
    }
}