#include "counter.h"
#include <assert.h>
#include <pthread.h>

void counter_init(counter_t *counter) {
    counter->count = 0;
    int rc = pthread_mutex_init(&counter->lock, NULL);
    assert(rc == 0);
}

void counter_reset(counter_t *counter) {
    pthread_mutex_lock(&counter->lock);
    counter->count = 0;
    pthread_mutex_unlock(&counter->lock);
}

int counter_getCount(counter_t *counter) {
    pthread_mutex_lock(&counter->lock);
    int count = counter->count;
    pthread_mutex_unlock(&counter->lock);
    return count;
}

void counter_increment(counter_t *counter) {
    pthread_mutex_lock(&counter->lock);
    counter->count++;
    pthread_mutex_unlock(&counter->lock);
}

void counter_decrement(counter_t *counter) {
    pthread_mutex_lock(&counter->lock);
    counter->count--;
    pthread_mutex_unlock(&counter->lock);
}
