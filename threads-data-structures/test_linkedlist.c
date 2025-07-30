#include <pthread.h>
#include <stdlib.h>

#include "linkedlist.h"

void *insertRoutine(void *params);

typedef struct {
    linkedlist_t *list;
    int value;
} thread_params;

int main() {
    linkedlist_t list;
    linkedlist_init(&list);

    for (int i = 10; i > 0; --i)
        linkedlist_insert(&list, i);

    linkedlist_print(&list);

    linkedlist_t list2;
    linkedlist_init(&list2);

    pthread_t threads[10];
    for (int i = 0; i < 10; i++) {
        thread_params p = {.list = &list2, .value = i};
        pthread_create(&threads[i], NULL, &insertRoutine, &p);
    }

    for (int i = 0; i < 10; i++)
        pthread_join(threads[i], NULL);

    linkedlist_print(&list2);
}

void *insertRoutine(void *params) {
    thread_params *p = params;
    linkedlist_insert(p->list, p->value);
    free(p);
    return NULL;
}
