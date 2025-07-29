#ifndef __APROX_COUNTER_H
#define __APROX_COUNTER_H

#include <pthread.h>

#define NUMCPUS 12
#define DEFAULT_THRESHOLD 5

typedef struct {
    int threshold;

    int global_count;
    pthread_mutex_t global_lock;

    int local_count[NUMCPUS];
    pthread_mutex_t local_lock[NUMCPUS];
} approx_counter_t;

void approxcounter_init(approx_counter_t *counter, int threshold);
void approxcounter_reset(approx_counter_t *counter);
void approxcounter_update(approx_counter_t *counter, int amount);
int approxcounter_getCount(approx_counter_t *counter);

#endif
