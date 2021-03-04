#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>

#include "cronkit_channel.h"
#include "cronkit_process.h"
#include "cronkit_signal.h"

struct cronkit_process_t *processes[128];
int cronkit_process_next_solt;
int cronkit_process_used_solt;



typedef struct {
    int age;
    char *name;
} user;

void init() {
    cronkit_process_next_solt = 0;
    cronkit_process_used_solt = -1;
}

void start_worker() {
    struct pollfd pfd;
    pfd.events = POLLIN;
    pfd.fd = processes[cronkit_process_used_solt]->channel[0];
    for(;;) {
        int ret;
        ret = poll(&pfd, 1, -1);
        if(ret > 0) {
            char buf[1024];
            ssize_t n = read(pfd.fd, buf, 1024-1);
            printf("pid: %d, n: %ld, message: %s\n", getpid(), n, buf);
        }        
    }
    sleep(1);
    exit(0);
}

void cronkit_process_on_exit() {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) { 
        printf("catch %d\n", pid);
    }
}

int main(int argc, char *argv[]) {
    int i=0;
    cronkit_signal_master_init();
    for(i=0; i<8; i++) {
        struct cronkit_process_t *process = cronkit_process_alloc();
        process->start_time = time(NULL);
        cronkit_process_channel_init(process);    

        pid_t pid;
        pid = fork();
        switch (pid) {
            case -1:
                exit(-1);
            case 0:
                process->pid = getpid();
                cronkit_process_used_solt = i;
                processes[cronkit_process_used_solt] = process;
                goto start_worker;
                break;
            default:
                process->pid = pid;
                processes[i] = process;
                break;
        }
    }
    
    int mfd = cronkit_signal_master_fd();
    
    struct pollfd pfd;
    pfd.events = POLL_IN;
    pfd.fd = mfd;

    int ret;
    for(;;) {
        ret = poll(&pfd, 1, 1000);
        
        int j;
        char buf[32];
        int size;
        for(j=0; j<8; j++) {
            size = sprintf(buf, "pid:%d:%d", processes[j]->pid, getpid());
            write(processes[j]->channel[1], buf, size+1);
        }
        if (ret > 0) {
            char c;
            read(mfd, &c, 1);
            int status;
            pid_t pid;
            switch (c) {
                case 'I':
                  exit(0);
                case 'C':
                   cronkit_process_on_exit();
            }
        }
    }
start_worker:
  start_worker();
}