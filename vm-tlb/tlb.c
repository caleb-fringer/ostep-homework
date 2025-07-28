#define _GNU_SOURCE
#include <bits/time.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

double getElapsedTime(struct timespec *start, struct timespec *end) {
    time_t sec = end->tv_sec - start->tv_sec;
    long nsec = end->tv_nsec - start->tv_nsec;

    if (nsec < 0) {
        sec -= 1;
        nsec += 1e9L;
    }

    return (double)sec * 1e9 + nsec;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: tlb num_pages num_trials\n");
        exit(EXIT_FAILURE);
    }

    int num_pages = atoi(argv[1]);
    int num_trials = atoi(argv[2]);
    if (num_pages == 0 || num_trials == 0) {
        printf("Please supply a non-zero value for both num_pages and "
               "num_trials\n");
        exit(EXIT_FAILURE);
    }

    cpu_set_t cpus;
    CPU_ZERO(&cpus);
    CPU_SET(1, &cpus);
    sched_setaffinity(0, sizeof(cpus), &cpus);

    long page_size = sysconf(_SC_PAGESIZE);

    size_t array_size = page_size * num_pages;
    size_t num_elements = array_size / sizeof(int);
    int *a = calloc(num_elements, sizeof(int));
    if (a == NULL) {
        printf("Error allocating memory for tests.\n");
        exit(EXIT_FAILURE);
    }

    int jump_size = page_size / sizeof(int);

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    for (int j = 0; j < num_trials; j++) {
        for (int i = 0; i < num_elements; i += jump_size) {
            a[i] += 1;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double elapsed_time = getElapsedTime(&start_time, &end_time);
    double avg_acces_time = elapsed_time / ((long)num_pages * num_trials);
    printf("%.5f\n", avg_acces_time);

    free(a);
}
