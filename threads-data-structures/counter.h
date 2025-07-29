#ifndef __COUNTER_H
#define __COUNTER_H

#include <pthread.h>

typedef struct __counter_t {
    int count;
    pthread_mutex_t lock;
} counter_t;

void init(counter_t *counter);
void reset(counter_t *counter);
int getCount(counter_t *counter);
void increment(counter_t *counter);
void decrement(counter_t *counter);

#endif
