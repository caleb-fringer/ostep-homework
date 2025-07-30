#include <assert.h>
#include <bits/time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "approxcounter.h"
#include "trial.h"

#define ITERATIONS 1000000
#define MAX_THREADS 12

int parseArgs(int argc, char *argv[]);
void runTrials(trial_params_t *params, int numThreads);

int main(int argc, char *argv[]) {
    int iterations = parseArgs(argc, argv);
    printf("Received %d iterations.\n\n", iterations);

    printf("Testing concurrent counter...\n");
    trial_params_t trial_1 = {.type = COUNTER,
                              .data_struct = &(concurrent_struct_t){},
                              .num_iterations = iterations,
                              .method =
                                  (void *(*)(void *)) & counter_increment};

    runTrials(&trial_1, MAX_THREADS);

    printf("Testing concurrent approximate counter...\n");
    trial_params_t trial_2 = {.type = APPROX_COUNTER,
                              .data_struct = &(concurrent_struct_t){},
                              .num_iterations = iterations,
                              .method =
                                  (void *(*)(void *)) & approxcounter_update};
    runTrials(&trial_2, MAX_THREADS);
    return 0;
}

int parseArgs(int argc, char *argv[]) {
    int iterations;
    if (argc == 1) {
        printf("Defaulting to 1m iterations.\n");
        iterations = ITERATIONS;
    } else if (argc == 2) {
        char *suffix = malloc(sizeof(char) * 2);

        if (sscanf(argv[1], "%d%1[kKmM]", &iterations, suffix) < 0) {
            perror(
                "Usage: trial [iterations]\n"
                "Iterations := integer optionally followed by one of [kKmM]");
        }

        switch (suffix[0]) {
        case 'k':
        case 'K':
            iterations *= 1000;
            break;
        case 'm':
        case 'M':
            iterations *= 1000000;
            break;
        default:
            perror("Ooopsy woopsy, a fucksy wucksy happened!");
        }
    } else {
        perror("Usage: trial [iterations]\n"
               "Iterations := integer optionally followed by one of [kKmM]");
    }

    return iterations;
}

/* Runs the given trial once for each number of threads in [1,numThreads],
 * printing results to stdout.
 */
void runTrials(trial_params_t *params, int num_threads) {
    for (int thread_count = 1; thread_count <= num_threads; thread_count++) {
        double elapsed_time = trial(thread_count, params);
        printf("It took %fs to run %d add operations on %d threads.\n",
               elapsed_time, params->num_iterations, thread_count);
    }
    printf("\n");
}
