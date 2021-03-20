#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>

#include "cronkit_signal.h"
#include "zend_signal.h"


static int sig_fd[2];

static int set_fd_unblock(int fd) {
    int flags = fcntl(fd, F_GETFL);
    if (flags < 0) {
        return -1;
    }
    flags |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, flags);
}

static void sig_handler(int sig_no)
{
    char c;
    switch (sig_no)
    {
    case SIGTERM:
        c = 'T';
        break;
    case SIGCHLD:
        c = 'C';
        break;
    case SIGINT:
        c = 'I';
        break;
    case SIGUSR1:
        c = '1';
        break;
    case SIGUSR2:
        c = '2';
        break;
    case SIGQUIT:
        c = 'Q';
        break;
    default:
        c = 'N';
        break;
    }
    write(sig_fd[1], &c, sizeof(c));
}

int cronkit_signal_master_init()
{
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sig_fd) != 0) {
        return -1;
    }
    if (set_fd_unblock(sig_fd[0]) == -1 || set_fd_unblock(sig_fd[1]) == -1) {
        return -1;
    }

    if (0 > fcntl(sig_fd[0], F_SETFD, FD_CLOEXEC) || 0 > fcntl(sig_fd[1], F_SETFD, FD_CLOEXEC)) {
        return -1;
    }

    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = sig_handler;

    sigfillset(&act.sa_mask);

    if (0 > sigaction(SIGTERM,  &act, 0) ||
	    0 > sigaction(SIGINT,   &act, 0) ||
	    0 > sigaction(SIGUSR1,  &act, 0) ||
	    0 > sigaction(SIGUSR2,  &act, 0) ||
	    0 > sigaction(SIGCHLD,  &act, 0) ||
	    0 > sigaction(SIGQUIT,  &act, 0)) {
		return -1;
	}
    return 0;
}

static void worker_sig_handler(int sn) {
    if(sn == SIGTERM) {
        exit(0);
    }
}


int cronkit_signal_worker_init()
{
    struct sigaction act, act_dfl;
    
    memset(&act, 0, sizeof(act));
    memset(&act_dfl, 0, sizeof(act_dfl));
    
    act.sa_handler = &worker_sig_handler;
    act.sa_flags |= SA_RESTART;

    act_dfl.sa_handler = SIG_DFL;

    close(sig_fd[0]);
    close(sig_fd[1]);

    if (0 > sigaction(SIGTERM,  &act_dfl,  0) ||
	    0 > sigaction(SIGINT,   &act_dfl,  0) ||
	    0 > sigaction(SIGUSR1,  &act_dfl,  0) ||
	    0 > sigaction(SIGUSR2,  &act_dfl,  0) ||
	    0 > sigaction(SIGCHLD,  &act_dfl,  0) ||
	    0 > sigaction(SIGQUIT,  &act,      0)) { 
            return -1;
        }    
    
    zend_signal_init();

    return 0;        
}

int cronkit_signal_master_fd() {
    return sig_fd[0];
}