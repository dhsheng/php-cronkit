#include <stdlib.h>
#include <string.h>

#include "cronkit_channel.h"
#include "cronkit_heap.h"
#include "cronkit_joblet.h"


struct cronkit_heap_t cronkit_joblet_heap;
struct cronkit_joblet_linked_list_t cronkit_joblet_list;


int cronkit_joblet_entry_cmp(const void *a, const void *b) {
    cronkit_joblet_entry_t *aa, *bb;
    aa = cronkit_joblet_entry_ptr(a);
    bb = cronkit_joblet_entry_ptr(b);
    return aa->next_start > bb->next_start ? 1 : 0;
}

int cronkit_joblet_entry_free(void *joblet) {
    if(joblet != NULL) {
        free(joblet);
    }
    return 0;
}

struct cronkit_joblet_t *cronkit_joblet_alloc() {
    struct cronkit_joblet_t *joblet;
    joblet = malloc(sizeof(struct cronkit_joblet_t));
    joblet->next = NULL;
    return joblet;
}

void cronkit_joblet_linked_list_add(
    struct cronkit_joblet_linked_list_t *list,
    struct cronkit_joblet_t *joblet
) {
    if(joblet == NULL || list == NULL) {
        return;
    }
    if(list->head == NULL) {
        list->head = joblet;
    } else {
        joblet->next = list->head;
        list->head = joblet;
    }
    list->size++;
}

cronkit_joblet_entry_t *cronkit_joblet_entry_build(struct cronkit_joblet_t *joblet) {

    cronkit_joblet_entry_t *joblet_entry = cronkit_joblet_entry_alloc();
    time_t now = time(NULL);

    const char *error;
    cron_parse_expr(joblet->expr, &(joblet_entry->cron_expr), &error);
    if (error != NULL) {
        return NULL;
    }

    strcpy(joblet_entry->joblet.expr, joblet->expr);
    strcpy(joblet_entry->joblet.handler_class, joblet->handler_class);
    strcpy(joblet_entry->joblet.name, joblet->name);

    joblet_entry->next_start = cron_next(&joblet_entry->cron_expr, now);
    joblet_entry->prev_start = 0;

    // struct tm *tm = localtime(&joblet_entry->next_start);
    // char ds[32];
    // strftime(ds, 31, "%Y-%m-%d %H:%M:%S", tm);

    // printf("cron: %s, name: %s, ps: %ld, next: %s\n", joblet_entry->joblet.expr,
    //    joblet_entry->joblet.name, joblet_entry->prev_start, ds);
    return joblet_entry;
}

void cronkit_joblet_list_cleanup(struct cronkit_joblet_linked_list_t *list) {
    struct cronkit_joblet_t *joblet, *tmp;
    joblet = list->head;
    while(joblet) {
        tmp = joblet;
        joblet = joblet->next;
        free(tmp);
    }
}


cronkit_joblet_entry_t *cronkit_joblet_entry_alloc() {
    cronkit_joblet_entry_t  *entry;
    entry = malloc(sizeof(cronkit_joblet_entry_t));
    return entry;
}

int cronkit_joblet_heap_init(int cap) {
    cronkit_heap_init(
        &cronkit_joblet_heap, 
        cap, 
        cronkit_joblet_entry_cmp, 
        cronkit_joblet_entry_free
    );
    return 0;
}

int cronkit_joblet_heap_cleanup(struct cronkit_heap_t *heap) {
    cronkit_heap_cleanup(heap);
    return 0;
}


int cronkit_joblet_list_init() {
    cronkit_joblet_linked_list_init(&cronkit_joblet_list);
    return 0;
}