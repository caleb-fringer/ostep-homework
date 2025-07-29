#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "trial.h"

/* Calculate the elapsed time between two structures. Return the answer in
 * seconds.
 */
double getElapsedTime(struct timespec *start, struct timespec *end) {
    time_t sec = end->tv_sec - start->tv_sec;
    long nsec = end->tv_nsec - start->tv_nsec;

    if (nsec < 0) {
        sec -= 1;
        nsec += 1e9L;
    }

    return (double)sec + nsec / 1e9;
}

/* A trial consists of initializing a concurrent data structure, creating n
 * threads, and executing K locking operations on each thread.
 *
 * The trial will be timed from the beginning of thread creation until the last
 * thread rejoins the main thread, and this value is returned as a double of
 * seconds elapsed using the MONOTONIC_RAW clock.
 *
 * This will give an approximate time, but it will also count the overhead
 * included in creating and destroying threads serially. So, as thread count
 * increases, this overhead should increase linearly.
 *
 * This function expects the number of threads, and a pointer to a
 * trial_params_t object which contains a pointer to the data structure, the
 * number of iterations to execute, and a function pointer to the method that is
 * being tested.
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

/*  The threadRoutine is the function run by each thread in trial. It simply
 *  performs the requested operation the requested number of times on the passed
 *  data structure.
 *
 *  The trial() functions passes the trial_params_t pointer to this routine,
 *  which is casted back into a trial_params_t and used to access the concurrent
 *  data structure and the operation being tested.
 */
void *threadRoutine(void *p) {
    trial_params_t *params = (trial_params_t *)p;
    for (int i = 0; i < params->num_iterations; i++) {
        params->method(params->data_struct);
    }
    return NULL;
}
