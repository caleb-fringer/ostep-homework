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

void init(approx_counter_t *counter, int threshold);
void reset(approx_counter_t *counter);
void update(approx_counter_t *counter, int amount);
int getCount(approx_counter_t *counter);

#endif
