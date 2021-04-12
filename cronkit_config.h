#ifndef CRONKIT_CONFIG_H
#define CRONKIT_CONFIG_H

typedef struct {
    int max_worker_num;
} cronkit_config_t;

extern cronkit_config_t global_cronkit_config;
#endif