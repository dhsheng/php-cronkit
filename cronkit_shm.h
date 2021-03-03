#ifndef CONSKIT_SHM_H
#define CONSKIT_SHM_H

void *cronkit_shm_alloc(size_t size);
int cronkit_shm_free(void *mem, size_t size);

#endif