#ifndef CRONKIT_SIGNAL_H
#define CRONKIT_SIGNAL_H

#include <signal.h>

int cronkit_signal_master_init();
int cronkit_signal_worker_init();
int cronkit_signal_master_fd();
#endif