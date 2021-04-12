#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/wait.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/pcre/php_pcre.h"
#include "ext/spl/spl_exceptions.h"

#include "cronkit_channel.h"
#include "cronkit_worker_process.h"
#include "cronkit_signal.h"
#include "cronkit_joblet.h"
#include "cronkit_heap.h"
#include "cronkit_event_loop.h"
#include "cronkit_main.h"


#define CRONKIT_MAX_FD 128
#define CRONKIT_MAX_WORKER_PROCESS 32
#define CRONKIT_JOB_MESSAGE_SIZE 1024

void handle_job_received(struct cronkit_event_loop_t *event_loop, int fd, void *data, int mask) {
    char buf[CRONKIT_JOB_MESSAGE_SIZE + 1];
    ssize_t nread;
    nread = read(fd, &buf, CRONKIT_JOB_MESSAGE_SIZE);
    if (nread <= 0) {
        return;
    }
    zend_class_entry *ce = zend_lookup_class(zend_string_init(buf, strlen(buf), 0));
    if (ce == NULL) {
        return ;
    }
    
    zval obj;
    if (SUCCESS != object_init_ex(&obj, ce)) {
        return;
    }

    zval retval, fn;

    ZVAL_STRING(&fn, "execute");                                                                                                 
    zend_fcall_info fci = empty_fcall_info;                                                                                   
    fci.size = sizeof(fci);                                                                                               
    fci.function_name = fn;                                                                                                  
    fci.params = NULL;                                                                                                      
    fci.param_count = 0;                                                                                                      
    fci.retval = &retval;                                                                                                     
    fci.object = Z_OBJ(obj);                                                                                                
    fci.no_separation = 1;  
    zend_call_function(&fci, NULL);
}


void cronkit_process_exit() {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) { 
        
        if(WIFEXITED(status)) {
            // @todo
        }
        if(WIFSIGNALED(status)) {
            // @todo
        }
        if(WIFSTOPPED(status)) {
            // @todo
        }
    }
}



int start_joblet_scheduler(struct cronkit_event_loop_t *elv) {
    if (elv == NULL) {
        return -1;
    }
    struct cronkit_heap_t *heap = &cronkit_joblet_heap;
    if(heap == NULL) {
        return -1;
    }
    void *p;
    time_t now;
    int i;
    for(;;) {
        p = cronkit_heap_peek(heap);
        if(p == NULL) {
            break;
        }
        now = time(NULL);
        cronkit_joblet_entry_t *entry = cronkit_joblet_ptr_cast(p);
        if(now < entry->next_start) {
            break;
        }
        time_t prev;        
        entry = cronkit_heap_pop(heap);
        /**
         *  @todo write to fd
         *  @todo worker_process 
         */
        for(i=0; i<CRONKIT_MAX_WORKER_PROCESS; i++) {
            write(cronkit_worker_processes[i]->channel[1], entry->joblet.handler_class, strlen(entry->joblet.handler_class)+1);
        }
        cronkit_joblet_entry_update(entry, now);
        cronkit_heap_put(heap, entry);
    }
    return 0;
}

void cronkit_exit_cleanup() {
    int i;
    // @todo
    for(i=0; i<CRONKIT_MAX_WORKER_PROCESS; i++) {
        kill(cronkit_worker_processes[i]->pid, SIGTERM);
    }
    exit(0);
}

void handle_worker_message(struct cronkit_event_loop_t *event_loop, int fd, void *data, int mask) {
    ssize_t size;
    char msg[CRONKIT_JOB_MESSAGE_SIZE+1];
    read(fd, &msg, CRONKIT_JOB_MESSAGE_SIZE);
}

void handle_signal_message(struct cronkit_event_loop_t *event_loop, int fd, void *data, int mask) {
    ssize_t size;
    char c;
    size = read(fd, &c, 1);
    if (size == -1) {
        return;
    }
    switch(c) {
        case 'I':
           cronkit_exit_cleanup();
           break;
        case 'C':
           cronkit_process_exit();
           break;
        case 'T':
           cronkit_process_exit();
           break;
        case '1':
        case '2':
           break;      
    }
}

int cronkit_worker_process_current_channel() {
    return cronkit_worker_process_current->channel[1]; 
}

int start_worker_process() {

    int fd;
    struct cronkit_event_loop_t *elv;

    // init worker process signal
    cronkit_signal_worker_init();
    
    elv = cronkit_event_loop_init(128);
    fd = cronkit_worker_process_current_channel();
    if(elv == NULL) {
        exit(-1);
    }
    elv->running = 1;
    cronkit_event_loop_add(elv, fd, CRONKIT_EVENT_READ, handle_job_received);
    return cronkit_event_loop_main(elv);
}


int cronkit_real_main(cronkit_config_t config) {

    int ret, mfd, i;
    struct cronkit_event_loop_t *elv;

    // init signal for master
    ret = cronkit_signal_master_init();
    
    // fork worker process
    pid_t pid;
    // @todo
    for(i=0; i<CRONKIT_MAX_WORKER_PROCESS; i++) {
        pid = cronkit_worker_process_fork(i);
        if (pid == 0) {
            goto start_worker_process;
        }
    }
 
    // init event loop
    elv = cronkit_event_loop_init(512);
       
    // setup joblet scheduler
    struct cronkit_timer_event_t te;
    te.next = NULL;
    te.handler = &start_joblet_scheduler;
    elv->timer_event = &te;

    // setup signal handler
    mfd = cronkit_signal_master_fd();
    cronkit_event_loop_add(elv, mfd, CRONKIT_EVENT_READ, handle_signal_message);

    elv->running = 1;
    return cronkit_event_loop_main(elv);

start_worker_process:
    start_worker_process();
}