#ifndef CRONKIT_JOBLET_H
#define CRONKIT_JOBLET_H

#include <time.h>

#include "cronkit_heap.h"
#include "cronkit_event_loop.h"
#include "ccronexpr/ccronexpr.h"

#define cronkit_joblet_ptr_cast(v)  ((cronkit_joblet_entry_t *)(v));

#define cronkit_joblet_linked_list_init(list) do { \
    (list)->head = NULL; \
    (list)->size = 0; \
} while (0)


struct cronkit_joblet_t {
    char name[128];
    char expr[64];
    char handler_class[256];
    struct cronkit_joblet_t *next;
};

struct cronkit_joblet_linked_list_t {
    struct cronkit_joblet_t *head;
    int size;
};

struct cronkit_joblet_t *cronkit_joblet_alloc();
void cronkit_joblet_linked_list_add(
    struct cronkit_joblet_linked_list_t *list,
    struct cronkit_joblet_t *joblet
);
void cronkit_joblet_list_cleanup(struct cronkit_joblet_linked_list_t *list);

typedef struct {
    struct cronkit_joblet_t joblet;
    cron_expr cron_expr;
    time_t next_start;
    time_t prev_start;
} cronkit_joblet_entry_t;

void cronkit_joblet_entry_update(cronkit_joblet_entry_t *entry, time_t time);
cronkit_joblet_entry_t *cronkit_joblet_entry_alloc();
cronkit_joblet_entry_t *cronkit_joblet_entry_build(struct cronkit_joblet_t *joblet);

int cronkit_joblet_list_init();
int cronkit_joblet_heap_init(int cap);
int cronkit_joblet_heap_cleanup(struct cronkit_heap_t *heap);

extern struct cronkit_heap_t cronkit_joblet_heap;
extern struct cronkit_joblet_linked_list_t cronkit_joblet_list;

#endif