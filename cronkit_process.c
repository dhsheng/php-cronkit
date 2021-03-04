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
