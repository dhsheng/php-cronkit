#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "cronkit_channel.h"
#include "cronkit_worker_process.h"


struct cronkit_worker_process_t *cronkit_worker_processes[128];
struct cronkit_worker_process_t *cronkit_worker_process_current;
int cronkit_worker_process_next_free_idx = 0;


int cronkit_worker_process_channel_init(struct cronkit_worker_process_t *process) {
    return cronkit_channel_init(process->channel);
}

pid_t cronkit_worker_process_fork(int solt) {

    struct cronkit_worker_process_t *process;
    process = malloc(sizeof(struct cronkit_worker_process_t));
    cronkit_worker_process_channel_init(process);

    process->solt = solt;
    process->start_time = time(NULL);

    pid_t pid;
    pid = fork();
    
    switch(pid) {
        case -1:
           return -1;
        case 0:
           process->pid = getpid();
           close(process->channel[0]);
           cronkit_worker_process_current = process;
           return 0;
        default:
           close(process->channel[1]);
           cronkit_worker_processes[solt] = process;
           cronkit_worker_processes[solt]->pid = pid;
           return pid;
    }
}