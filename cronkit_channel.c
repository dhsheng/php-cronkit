#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#include "cronkit_channel.h"

static int cronkit_fd_unblocking(int fd) {
    int flags = fcntl(fd, F_GETFL);
    if (flags < 0) {
        return -1;
    }
    flags |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, flags); 
}

int cronkit_channel_init(int ch[2]) {
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, ch) != 0) {
        return -1;
    }
    
    if (cronkit_fd_unblocking(ch[0]) == -1 
        || cronkit_fd_unblocking(ch[1]) == -1) {
        return -1;
    }

    if (0 > fcntl(ch[0], F_SETFD, FD_CLOEXEC) 
        || 0 > fcntl(ch[1], F_SETFD, FD_CLOEXEC)) {
        return -1;
    }
    return 0;
}