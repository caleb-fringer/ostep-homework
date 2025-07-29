#include <assert.h>
#include <bits/time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "trial.h"

#define ITERATIONS 1000000

int parseArgs(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    int iterations = parseArgs(argc, argv);
    printf("Received %d iterations.\n", iterations);

    trial_params_t trial_1 = {.type = COUNTER,
                              .data_struct = &(concurrent_struct_t){},
                              .num_iterations = iterations,
                              .method = (void *(*)(void *)) & increment};

    double time = trial(12, &trial_1);
    printf("It took %fs to run %d add operations.\n", time,
           trial_1.num_iterations);
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
