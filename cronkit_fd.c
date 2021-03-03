#include <fcntl.h>

#include "cronkit_fd.h"

int cronkit_fd_unblocking(int fd) {
    int flags = fcntl(fd, F_GETFL);
    if (flags < 0) {
        return -1;
    }
    flags |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, flags); 
}