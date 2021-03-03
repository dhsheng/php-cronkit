#include <sys/mman.h>
#include <stdlib.h>
#include <stddef.h>

#include "cronkit_shm.h"


void *cronkit_shm_alloc(size_t size) {
    void *m;
    m = mmap(0, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (!m) {
        return NULL;
    }
    return m;
}

int cronkit_shm_free(void *mem, size_t size) {
    if (munmap(mem, size) == -1) {
        return -1;
	}
    return 0;
}