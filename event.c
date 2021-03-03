#ifndef CRONKIT_EVENT_H
#define CRONKIT_EVENT_H

#define CRONKIT_EVENT_NONE 0
#define CRONKIT_EVENT_READ 1
#define CRONKIT_EVENT_WRITE 2

struct cronkit_event_loop_t;

typedef void cronkit_event_handler(struct cronkit_event_loop_t *event_loop, int fd, void *data, int mask);

struct cronkit_event_t {
    int fd;
    int mask;
    cronkit_event_handler *read;
    cronkit_event_handler *write;
};


int cronkit_event_fire(struct cronkit_event_t *event);

int cronkit_event_loop_wait(int timeout);
int cronkit_event_loop_add(struct cronkit_event_loop_t *event_loop, int fd, int mask);
int cronkit_event_loop_del(struct cronkit_event_loop_t *event_loop, int fd, int mask);
int cronkit_event_loop_init(int maxfd);

#endif