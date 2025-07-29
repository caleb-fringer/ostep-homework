#ifndef __TRIAL_H
#define __TRIAL_H

#include <bits/time.h>

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

#endif
