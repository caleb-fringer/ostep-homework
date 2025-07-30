#include "linkedlist.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void linkedlist_init(linkedlist_t *list) {
    pthread_mutex_init(&list->dummy.lock, NULL);
    list->dummy.next = NULL;
    list->head = NULL;
    list->length = 0;
}

int linkedlist_search(linkedlist_t *list, int target) {
    node_t *current = &list->dummy;
    pthread_mutex_lock(&current->lock);

    int result = -1;
    while (current->next) {
        pthread_mutex_lock(&current->next->lock);
        node_t *next = current->next;
        pthread_mutex_unlock(&current->lock);
        current = next;

        if (current->data == target) {
            result = 0;
            break;
        }
    }

    pthread_mutex_unlock(&current->lock);
    return result;
}

void linkedlist_insert(linkedlist_t *list, int value) {
    node_t *new_node = malloc(sizeof(node_t));
    new_node->data = value;
    pthread_mutex_init(&new_node->lock, NULL);

    pthread_mutex_lock(&new_node->lock);
    pthread_mutex_lock(&list->dummy.lock);
    list->length++;
    new_node->next = list->dummy.next;
    list->dummy.next = new_node;
    pthread_mutex_unlock(&list->dummy.lock);
    pthread_mutex_unlock(&new_node->lock);
}

void linkedlist_print(linkedlist_t *list) {
    node_t *current = &list->dummy;
    pthread_mutex_lock(&current->lock);

    while (current->next) {
        pthread_mutex_lock(&current->next->lock);
        node_t *next = current->next;
        pthread_mutex_unlock(&current->lock);
        current = next;

        printf("%d, ", current->data);
    }
    pthread_mutex_unlock(&current->lock);

    printf("\n");
}
