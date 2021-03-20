#ifndef CRONKIT_EVENT_LOOP_H
#define CRONKIT_EVENT_LOOP_H

#define CRONKIT_EVENT_NONE 0
#define CRONKIT_EVENT_READ 1
#define CRONKIT_EVENT_WRITE 2

struct cronkit_event_t;
struct cronkit_event_fired_t;
struct cronkit_event_loop_t;
struct cronkit_timer_event_t;


typedef void cronkit_event_handler(
    struct cronkit_event_loop_t *event_loop, 
    int fd, void *data, int mask);

typedef int cronkit_timer_event_handler(
    struct cronkit_event_loop_t *event_loop);

struct cronkit_event_t {
    int fd;
    int mask;
    cronkit_event_handler *read;
    cronkit_event_handler *write;
};

struct cronkit_event_fired_t {
    int fd;
    int mask;
};

struct cronkit_timer_event_t {
    struct cronkit_timer_event_t *next;
    cronkit_timer_event_handler *handler;
};


struct cronkit_event_loop_t {
    int max_fd;
    int running;
    struct cronkit_event_t *events;
    struct cronkit_event_fired_t *fired;
    void *dialect;
    struct cronkit_timer_event_t *timer_event;
};

struct cronkit_event_loop_dialect_poll_t {
    int npfds;
    int next_free_solt;
    struct pollfd *pfds;
};

int cronkit_event_loop_wait(struct cronkit_event_loop_t *event_loop, long timeout);
int cronkit_event_loop_main(struct cronkit_event_loop_t *event_loop);
int cronkit_event_loop_add(struct cronkit_event_loop_t *event_loop, int fd, int mask, 
                            cronkit_event_handler *handler);
int cronkit_event_loop_del(struct cronkit_event_loop_t *event_loop, int fd, int mask);
struct cronkit_event_loop_t *cronkit_event_loop_init(int max_fd);

#endif