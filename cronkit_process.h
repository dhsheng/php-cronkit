#ifndef CRONKIT_PROCESS_H
#define CRONKIT_PROCESS_H

#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#include "cronkit_channel.h"

struct cronkit_process_t {
    pid_t pid;
    time_t start_time;
    int channel[2];
    int is_parent;
    int solt;
};

int cronkit_process_channel_init(struct cronkit_process_t *process);

struct cronkit_process_t *cronkit_process_alloc();
struct cronkit_process_t *cronkit_process_fork();

extern struct cronkit_process_t cronkit_processes[1024];
extern int cronkit_process_next_free_solt;

#endif
