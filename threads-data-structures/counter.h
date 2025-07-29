#ifndef __COUNTER_H
#define __COUNTER_H

#include <pthread.h>

typedef struct __counter_t {
    int count;
    pthread_mutex_t lock;
} counter_t;

void counter_init(counter_t *counter);
void counter_reset(counter_t *counter);
int counter_getCount(counter_t *counter);
void counter_increment(counter_t *counter);
void counter_decrement(counter_t *counter);

#endif
