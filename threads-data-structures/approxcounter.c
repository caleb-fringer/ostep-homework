#include <assert.h>
#include <pthread.h>

#include "approxcounter.h"

/* Takes an approx_counter_t struct and a threshold value. Initializes all
 * local and global counts to 0 and initializes all locks.
 *
 * If threshold param is < 1, threshold member is initialized to a default
 * value.
 */
void init(approx_counter_t *counter, int threshold) {
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
void reset(approx_counter_t *counter) {
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
