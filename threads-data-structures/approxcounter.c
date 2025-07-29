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
