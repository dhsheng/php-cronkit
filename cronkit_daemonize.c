#include <unistd.h>
#include <fcntl.h>

#include "cronkit_daemonize.h"

int cronkit_daemonize() {

    switch (fork()) {
        case -1:
          return -1;
        case 0:
          break;   
        default:
          return 0; // parent
    }

    if (-1 == setsid()) {
        return -1;
    }

    int fd;
    fd = open("/dev/null", O_RDWR, 0);
    if (-1 == fd) {
        return -1;
    }
    
    if (dup2(fd, STDIN_FILENO) < 0) {
        return -1;
    }

    if (dup2(fd, STDOUT_FILENO) < 0) {
        return -1;
    }

    if (dup2(fd, STDERR_FILENO) < 0) {
        return -1;
    }
    
    if (fd > STDERR_FILENO) {
        return -1;
    }
    return 0;
}