#include <assert.h>
#include <bits/time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "counter.h"

typedef union {
    counter_t counter;
} concurrent_struct_t;

typedef enum { COUNTER, LINKED_LIST, QUEUE, B_TREE } data_struct_types;

typedef struct {
    data_struct_types type;
    concurrent_struct_t *data_struct;
    int num_iterations;
    void *(*method)(void *);
} trial_params_t;

double getElapsedTime(struct timespec *start, struct timespec *end);
double trial(int num_threads, trial_params_t *params);
void *threadRoutine(void *params);

int main() {
    trial_params_t trial_1 = {.type = COUNTER,
                              .data_struct = &(concurrent_struct_t){},
                              .num_iterations = 10000000,
                              .method = (void *(*)(void *)) & increment};
    double time = trial(12, &trial_1);
    printf("It took %fs to run %d add operations.\n", time,
           trial_1.num_iterations);
    return 0;
}

double getElapsedTime(struct timespec *start, struct timespec *end) {
    time_t sec = end->tv_sec - start->tv_sec;
    long nsec = end->tv_nsec - start->tv_nsec;

    if (nsec < 0) {
        sec -= 1;
        nsec += 1e9L;
    }

    return (double)sec + nsec / 1e9;
}

/* I need to run many trials for each data structure. A trial consists of
 * executing a data structure operation some number of times K on each of some
 * number of threads n. Each thread will execute K iterations of the operation.
 *
 * In the main thread, I will initialzie the data structure,
 * start a timer before the threads are created, iteratively create the threads
 * while passing the data structure and the value of K, and stop the timer when
 * the last join completes. This will give an approximate time, but it will
 * also count the overhead included in creating and destroying threads serially.
 * So, as thread count increases, this overhead should increase linearly.
 *
 * For the thread routine, I need to create a function that takes the integer
 * passed, K, the data structure used, and a function pointer to the operation
 * I want to perform on it.
 */
double trial(int num_threads, trial_params_t *params) {
    // Initialize the data structure
    switch (params->type) {
    case COUNTER:
        init((counter_t *)params->data_struct);
        break;
    default:
        perror("Error initializng data structure, unknown or unimplemented "
               "type.\n");
    }

    pthread_t *threads = malloc(sizeof(pthread_t) * num_threads);

    struct timespec begin, end;
    assert(clock_gettime(CLOCK_MONOTONIC_RAW, &begin) == 0);

    for (int t = 0; t < num_threads; t++) {
        pthread_create(&threads[t], NULL, &threadRoutine, params);
    }

    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL);
    }

    assert(clock_gettime(CLOCK_MONOTONIC_RAW, &end) == 0);
    free(threads);

    return getElapsedTime(&begin, &end);
}

void *threadRoutine(void *p) {
    trial_params_t *params = (trial_params_t *)p;
    for (int i = 0; i < params->num_iterations; i++) {
        params->method(params->data_struct);
    }
    return NULL;
}
