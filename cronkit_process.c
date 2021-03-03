#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "cronkit_process.h"
#include "cronkit_channel.h"


struct cronkit_process_t cronkit_processes[1024];
int cronkit_process_next_free_solt = 0;


int cronkit_process_channel_init(struct cronkit_process_t *process) {
    return cronkit_channel_init(process->channel);
}

struct cronkit_process_t  *cronkit_process_alloc() {
    struct cronkit_process_t *process;
    process = malloc(sizeof(struct cronkit_process_t));
    process->solt = -1;
    return process;
}

struct cronkit_process_t *cronkit_process_fork() {

    struct cronkit_process_t *process = cronkit_process_alloc();
    process->start_time = time(NULL);
    cronkit_process_channel_init(process);    

    pid_t pid;
    pid = fork();
    switch (pid) {
        case -1:
            return NULL;
        case 0:
            process->pid = getpid();
            process->is_parent = 0;
            break;
        default:
            process->pid = pid;
            process->is_parent = 1;
            break;
    }
    return process;

}