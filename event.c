#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "cronkit_event_loop.h"

struct cronkit_event_loop_t *event_loop = NULL;

int say_hello(struct cronkit_event_loop_t *event_loop) {
    printf("say hello %ld\n", time(NULL));
}

int main() {
    event_loop = cronkit_event_loop_init(128);
    if (event_loop == NULL) {
        printf("error\n");
        exit(-1);
    }
    event_loop->running = 1;

    struct cronkit_timer_event_t te;
    te.handler = &say_hello;
    te.next = NULL;
    event_loop->timer_event = &te;

    cronkit_event_loop_main(event_loop);
    return 0;
}
