#ifndef __LINKED_LIST_H
#define __LINKED_LIST_H

#include <pthread.h>

typedef struct _node_t {
    int data;
    struct _node_t *next;
    pthread_mutex_t lock;
} node_t;

typedef struct {
    int length;
    node_t dummy, *head;
} linkedlist_t;

void linkedlist_init(linkedlist_t *list);
int linkedlist_search(linkedlist_t *list, int target);
void linkedlist_insert(linkedlist_t *list, int value);
void linkedlist_print(linkedlist_t *list);

#endif
