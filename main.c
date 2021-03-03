#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "cronkit_channel.h"
#include "cronkit_process.h"
#include "cronkit_signal.h"

struct cronkit_process_t *process[128];
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
    // struct pollfd pfd;
    // pfd.events = POLLIN;
    // pfd.fd = process[cronkit_process_used_solt]->channel[0];
    // for(;;) {
    //     int ret;
    //     ret = poll(&pfd, 1, -1);
    //     if(ret > 0) {
    //         char buf[1024];
    //         ssize_t n = read(pfd.fd, buf, 1024-1);
    //         printf("pid: %d, n: %ld, message: %s\n", getpid(), n, buf);
    //     }        
    // }
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
    char *oldname = argv[0];
    argv[0] = "hello";
    execv(oldname, argv);
    sleep(1000);
    int i=0;
    cronkit_signal_master_init();
    for(i=0; i<8; i++) {
        process[i] = cronkit_process_fork();
        process[i]->solt = i;
        if (!process[i]->is_parent) {
            cronkit_process_used_solt = i;
            printf("pid: %d, solt: %d\n", getpid(), i);
            goto start_worker;
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
        for(j=0; j<8; j++) {
            
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