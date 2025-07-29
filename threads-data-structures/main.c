#include <assert.h>
#include <bits/time.h>
#include <pthread.h>
#include <stdio.h>

#include "trial.h"

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
