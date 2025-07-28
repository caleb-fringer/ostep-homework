#include "counter.h"
#include <assert.h>
#include <pthread.h>

void init(counter_t *counter) {
    counter->count = 0;
    int rc = pthread_mutex_init(&counter->lock, NULL);
    assert(rc == 0);
}

void reset(counter_t *counter) {
    pthread_mutex_lock(&counter->lock);
    counter->count = 0;
    pthread_mutex_unlock(&counter->lock);
}

int getCount(counter_t *counter) {
    pthread_mutex_lock(&counter->lock);
    int count = counter->count;
    pthread_mutex_unlock(&counter->lock);
    return count;
}

void increment(counter_t *counter) {
    pthread_mutex_lock(&counter->lock);
    counter->count++;
    pthread_mutex_unlock(&counter->lock);
}

void decrement(counter_t *counter) {
    pthread_mutex_lock(&counter->lock);
    counter->count--;
    pthread_mutex_unlock(&counter->lock);
}
