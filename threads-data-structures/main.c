#include <assert.h>
#include <stdio.h>
#include <time.h>

#include "counter.h"

typedef union {
    counter_t counter;
} concurrent_struct_t;

typedef struct {
    concurrent_struct_t *data_struct;
    int num_threads;
    int num_iterations;
    void *(*method)(void *);
} trial_params_t;

double getElapsedTime(struct timespec *start, struct timespec *end);
double trial(trial_params_t *params);

int main() {
    struct timespec begin_time;
    assert(clock_gettime(CLOCK_MONOTONIC_RAW, &begin_time) == 0);

    printf("Time: %lds %ldns\n", begin_time.tv_sec, begin_time.tv_nsec);
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
