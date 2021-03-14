#ifndef CRONKIT_PROCESS_H
#define CRONKIT_PROCESS_H

#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#include "cronkit_channel.h"

struct cronkit_worker_process_t {
    pid_t pid;
    time_t start_time;
    int channel[2];
    int solt;
};

int cronkit_worker_process_channel_init(struct cronkit_worker_process_t *process);
pid_t cronkit_worker_process_fork(int slot);

extern struct cronkit_worker_process_t *cronkit_worker_processes[128];
extern struct cronkit_worker_process_t *cronkit_worker_process_current;
extern int cronkit_worker_process_next_free_idx;

#endif
