#define _GNU_SOURCE

#include <assert.h>
#include <pthread.h>
#include <sched.h>

#include "approxcounter.h"

/* Takes an approx_counter_t struct and a threshold value. Initializes all
 * local and global counts to 0 and initializes all locks.
 *
 * If threshold param is < 1, threshold member is initialized to a default
 * value.
 */
void approxcounter_init(approx_counter_t *counter, int threshold) {
    if (threshold < 1) {
        counter->threshold = DEFAULT_THRESHOLD;
    } else {
        counter->threshold = threshold;
    }

    counter->global_count = 0;
    assert(pthread_mutex_init(&counter->global_lock, NULL) == 0);

    for (int i = 0; i < NUMCPUS; i++) {
        counter->local_count[i] = 0;
        assert(pthread_mutex_init(&counter->local_lock[i], NULL) == 0);
    }
}

/* Reset all counts. Must grab all the locks simultaneously, so this operation
 * may be very slow.
 */
void approxcounter_reset(approx_counter_t *counter) {
    pthread_mutex_lock(&counter->global_lock);
    for (int i = 0; i < NUMCPUS; i++)
        pthread_mutex_lock(&counter->local_lock[i]);

    counter->global_count = 0;
    for (int i = 0; i < NUMCPUS; i++)
        counter->local_count[i] = 0;

    for (int i = 0; i < NUMCPUS; i++)
        pthread_mutex_unlock(&counter->local_lock[i]);
    pthread_mutex_unlock(&counter->global_lock);
}

/* Update the approximate counter. First, update the local count atomically.
 * Then, if the local count equals/exceeds the threshold (either positive or
 * negative), atomically update the global count and reset the local count to 0.
 */
void approxcounter_update(approx_counter_t *counter, int amount) {
    int cpu = sched_getcpu();

    pthread_mutex_lock(&counter->local_lock[cpu]);
    counter->local_count[cpu] += amount;

    // If we pass the threshold in the positive or negative direction...
    if (counter->local_count[cpu] >= counter->threshold ||
        counter->local_count[cpu] <= -counter->threshold) {

        // Update global count.
        pthread_mutex_lock(&counter->global_lock);
        counter->global_count += counter->local_count[cpu];
        pthread_mutex_unlock(&counter->global_lock);

        // Reset the local count to 0.
        counter->local_count[cpu] = 0;
    }

    pthread_mutex_unlock(&counter->local_lock[cpu]);
}

/* Return the (approximate) count from the global counter.
 */
int approxcounter_getCount(approx_counter_t *counter) {
    pthread_mutex_lock(&counter->global_lock);
    int count = counter->global_count;
    pthread_mutex_unlock(&counter->global_lock);
    return count;
}
