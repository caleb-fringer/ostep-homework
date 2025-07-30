#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "approxcounter.h"
#include "trial.h"

#define NUMITER 1000000

void testInitCounter(approx_counter_t *counter);
void testResetCounter(approx_counter_t *counter);
void testApproxCounterSingleThread(approx_counter_t *counter, int iterations);
void testApproxCounterMultiThread(approx_counter_t *counter, int iterations,
                                  int num_threads);

int main() {
    approx_counter_t counter;
    testInitCounter(&counter);
    testApproxCounterSingleThread(&counter, NUMITER);
    testResetCounter(&counter);
    testApproxCounterMultiThread(&counter, NUMITER, 12);
}

void testInitCounter(approx_counter_t *counter) {
    approxcounter_init(counter, 0);

    assert(counter->threshold == 5);
    assert(counter->global_count == 0);
    for (int i = 0; i < NUMCPUS; i++)
        assert(counter->local_count[i] == 0);
    printf("TestInitCounter succeeded.\n");
}

void testResetCounter(approx_counter_t *counter) {
    approxcounter_reset(counter);

    assert(counter->global_count == 0);
    for (int i = 0; i < NUMCPUS; i++)
        assert(counter->local_count[i] == 0);
    printf("TestResetCounter succeeded.\n");
}

void testApproxCounterSingleThread(approx_counter_t *counter, int iterations) {
    for (int i = 0; i < iterations; i++)
        approxcounter_update(counter, 1);
    int tolerance = (counter->threshold - 1) * NUMCPUS;
    int approx_count = approxcounter_getCount(counter);
    assert(iterations - tolerance <= approx_count &&
           approx_count <= iterations);
    printf("TestApproxCounterSingleThread succeeded. Global count: %d\n",
           approx_count);
}

typedef struct {
    approx_counter_t *counter;
    int iterations;
} thread_params_t;

void *updateCountRoutine(void *args) {
    thread_params_t *params = args;
    for (int i = 0; i < params->iterations; i++)
        approxcounter_update(params->counter, 1);
    return NULL;
}
void testApproxCounterMultiThread(approx_counter_t *counter, int iterations,
                                  int num_threads) {
    pthread_t *threads = malloc(sizeof(pthread_t) * num_threads);
    thread_params_t params = {.counter = counter, .iterations = iterations};

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, &updateCountRoutine, &params);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    int approx_count = approxcounter_getCount(counter);
    int tolerance = (NUMCPUS - 1) * counter->threshold;
    int expected = iterations * num_threads;

    assert(expected - tolerance <= approx_count && approx_count <= expected);

    pthread_mutex_lock(&counter->global_lock);
    for (int i = 0; i < NUMCPUS; i++)
        pthread_mutex_lock(&counter->local_lock[i]);

    int true_count = counter->global_count;
    for (int i = 0; i < NUMCPUS; i++)
        true_count += counter->local_count[i];

    assert(true_count == expected);

    pthread_mutex_unlock(&counter->global_lock);
    for (int i = 0; i < NUMCPUS; i++)
        pthread_mutex_unlock(&counter->local_lock[i]);

    printf("TestApproxCounterMultiThread succeeded. Global count: %d\n",
           approx_count);
}
