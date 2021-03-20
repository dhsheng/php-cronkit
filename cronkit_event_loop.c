#include <poll.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cronkit_event_loop.h"


static int cronkit_event_loop_dialect_poll_init(struct cronkit_event_loop_t *event_loop) {

    struct cronkit_event_loop_dialect_poll_t *dialect;
    dialect = malloc(sizeof(struct cronkit_event_loop_dialect_poll_t));
    if(dialect == NULL) {
        return -1;
    }
    dialect->npfds = 0;
    dialect->next_free_solt = 0;
    dialect->pfds = malloc(sizeof(struct pollfd) * event_loop->max_fd);
    memset(dialect->pfds, 0, sizeof(struct pollfd) * event_loop->max_fd);
    if(dialect->pfds == NULL) {
        free(dialect);
        return -1;
    }
    int i;
    for(i=0; i<event_loop->max_fd; i++) {
        dialect->pfds[i].fd = -1;
        dialect->pfds[i].events = CRONKIT_EVENT_NONE;
        dialect->pfds[i].revents = CRONKIT_EVENT_NONE;
    }
    event_loop->dialect = dialect;
    return 0;
}

static int cronkit_event_loop_dialect_poll_cleanup(struct cronkit_event_loop_t *event_loop) {
    if (NULL == event_loop->dialect) {
        return  -1;
    }
    struct cronkit_event_loop_dialect_poll_t  *dialect;
    dialect = (struct cronkit_event_loop_dialect_poll_t  *)event_loop->dialect;
    if(dialect->pfds == NULL) {
        return -1;
    }
    free(dialect->pfds);
    free(dialect);
    return 0;
}

static int cronkit_event_loop_dialect_poll_add(struct cronkit_event_loop_t *event_loop, int fd, int mask) {
    if (event_loop == NULL || 
        event_loop->dialect == NULL || 
        event_loop->max_fd <= fd) {
        return -1;
    }
    struct cronkit_event_loop_dialect_poll_t *dialect;
    dialect = (struct cronkit_event_loop_dialect_poll_t *)event_loop->dialect;
    int solt = dialect->next_free_solt++;
    dialect->pfds[solt].fd = fd;
    dialect->npfds++;
    if (mask & CRONKIT_EVENT_READ) {
        dialect->pfds[solt].events |= POLLIN;
    }
    if (mask & CRONKIT_EVENT_WRITE) {
        dialect->pfds[solt].events |= POLLOUT;
    }
    return 0;
}

static int cronkit_event_loop_dialect_poll_del(struct cronkit_event_loop_t *event_loop, int fd, int mask) {
    if (event_loop == NULL || 
        event_loop->dialect == NULL || 
        event_loop->max_fd <= fd) {
        return -1;
    }
    struct cronkit_event_loop_dialect_poll_t *dialect;
    dialect = (struct cronkit_event_loop_dialect_poll_t *)event_loop->dialect;
    
    int i;
    for(i=0; i<dialect->npfds; i++) {
        if (dialect->pfds[i].fd == fd) {
            if (mask & CRONKIT_EVENT_READ) {
                dialect->pfds[i].events &= (~POLLIN);
            }
            if (mask & CRONKIT_EVENT_WRITE) {
                 dialect->pfds[i].events &= (~POLLOUT);
            }
            if (dialect->pfds[i].events == 0) {
                dialect->pfds[i].events = 0;
                dialect->pfds[i].fd = -1;
                dialect->pfds[i].revents = 0;
                dialect->next_free_solt = i;
            }
            break;
        }
    }
    return 0;
}

static int cronkit_event_loop_dialect_poll_wait(struct cronkit_event_loop_t *event_loop, long timeout) {
    if(event_loop->max_fd <= 0) {
        return -1;
    }
    int ret;
    struct cronkit_event_loop_dialect_poll_t  *dialect; 
    dialect = (struct cronkit_event_loop_dialect_poll_t*)event_loop->dialect;

    ret = poll(dialect->pfds, dialect->npfds, timeout);

    if(ret == -1 && errno != EINTR) {
        return -1;
    }

    if (ret > 0) {
        int i;
        for(i=0; i<dialect->npfds; i++) {
            struct pollfd pfd = dialect->pfds[i];
            if (pfd.revents & POLLIN) {
                event_loop->fired[i].fd = pfd.fd;
                event_loop->fired[i].mask |= CRONKIT_EVENT_READ;
            }
            if (pfd.revents & POLLOUT) {
                event_loop->fired[i].fd = pfd.fd;
                event_loop->fired[i].mask |= CRONKIT_EVENT_WRITE;
            }
        }
    }
    return ret;
}


int cronkit_event_loop_wait(struct cronkit_event_loop_t *event_loop, long timeout) {
    return cronkit_event_loop_dialect_poll_wait(event_loop, timeout);
}

int cronkit_event_loop_main(struct cronkit_event_loop_t *event_loop) {
    while(event_loop->running) {
        int ret;
        ret = cronkit_event_loop_dialect_poll_wait(event_loop, 1000);
        struct cronkit_timer_event_t *event = event_loop->timer_event;
        while(event) {
            if(event->handler) {
                event->handler(event_loop);
            }
            event = event->next;
        }
        int i;
        for(i=0; i<ret; i++) {
            struct cronkit_event_t *event;
            struct cronkit_event_fired_t *fired; 
            fired = &event_loop->fired[i];
            event = &event_loop->events[fired->fd];
            if (fired->mask & CRONKIT_EVENT_READ && event->read) {
                event->read(event_loop, fired->fd, NULL, fired->mask);
            }
            if (fired->mask & CRONKIT_EVENT_WRITE && event->write) {
                event->write(event_loop, fired->fd, NULL, fired->mask);
            }
        }
    }
}

int cronkit_event_loop_add(struct cronkit_event_loop_t *event_loop, int fd, int mask, cronkit_event_handler *handler) {
    if (event_loop == NULL || mask & CRONKIT_EVENT_NONE) {
        return -1;
    }
    int ret;
    ret = cronkit_event_loop_dialect_poll_add(event_loop, fd, mask);
    if (ret < 0) {
        return -1;
    }
    event_loop->events[fd].fd = fd;

    if (mask & CRONKIT_EVENT_READ) {
        event_loop->events[fd].mask |= CRONKIT_EVENT_READ;   
        event_loop->events[fd].read = handler;
    }
    if (mask & CRONKIT_EVENT_WRITE) {
        event_loop->events[fd].mask |= CRONKIT_EVENT_WRITE;
        event_loop->events[fd].write = handler;
    }
    return 0;
}

int cronkit_event_loop_del(struct cronkit_event_loop_t *event_loop, int fd, int mask) {
    if (event_loop == NULL) {
        return -1;
    }
    return cronkit_event_loop_dialect_poll_del(event_loop, fd, mask);
}

struct cronkit_event_loop_t *cronkit_event_loop_init(int max_fd) {

    struct cronkit_event_loop_t *event_loop;
    event_loop = malloc(sizeof(struct cronkit_event_loop_t));
    if (event_loop == NULL) {
        return NULL;
    }
    
    event_loop->max_fd = max_fd;

    int ret;
    ret = cronkit_event_loop_dialect_poll_init(event_loop);
    if(ret == -1) {
        free(event_loop);
        return NULL;
    }

    int i;
    event_loop->timer_event = NULL;
    event_loop->events = malloc(sizeof(struct cronkit_event_t) * max_fd);
    if (event_loop->events == NULL) {
        cronkit_event_loop_dialect_poll_cleanup(event_loop);
        free(event_loop);
        return NULL;
    }
    for(i=0; i<max_fd; i++) {
        event_loop->events[i].fd = -1;
        event_loop->events[i].mask = CRONKIT_EVENT_NONE;
    }

    event_loop->fired = malloc(sizeof(struct cronkit_event_fired_t) * max_fd);
    if (event_loop->fired == NULL) {
        cronkit_event_loop_dialect_poll_cleanup(event_loop);
        free(event_loop);
        return NULL;
    }
    for(i=0; i<max_fd; i++) {
        event_loop->fired[i].fd = -1;
        event_loop->fired[i].mask = CRONKIT_EVENT_NONE;
    }
    return event_loop;
}
