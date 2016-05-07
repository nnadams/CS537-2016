#include <stdlib.h>
#include "linkedlist.h"

list_t *ll_init(int size) {
    list_t *new = malloc(sizeof(list_t));
    if (new == NULL)
        return NULL;

    new->size = 0;
    new->head = NULL;
    new->tail = NULL;

    if (size > 0)
        new->maxsize = size;
    else
        new->maxsize = 0;

    return new;
}

void ll_destory(list_t *list) {
    list_node_t *cur = list->head;
    list_node_t *tmp;

    while(cur != NULL) {
        tmp = cur->next;
        free(cur->data);
        free(cur);
        cur = tmp;
    }

    free(list);
}

int ll_insert_front(list_t *list, const void *data) {
    if (list->maxsize > 0 && list->maxsize == list->size)
        return -1;

    list_node_t *new = malloc(sizeof(list_node_t));
    if (new == NULL)
        return -1;

    new->data = (void *)data;

    if (list->size == 0) {
        list->head = new;
        list->tail = new;
        new->next = NULL;
    }
    else {
        new->next = list->head;
        list->head = new;
    }

    list->size++;
    return 0;
}

int ll_insert_end(list_t *list, const void *data) {
    if (list->maxsize > 0 && list->maxsize == list->size)
        return -1;

    list_node_t *new = malloc(sizeof(list_node_t));
    if (new == NULL)
        return -1;

    new->data = (void *)data;

    if (list->size == 0) {
        list->head = new;
        list->tail = new;
        new->next = NULL;
    }
    else {
        list->tail->next = new;
        new->next = NULL;
        list->tail = new;
    }

    list->size++;
    return 0;
}

void *ll_pop_front(list_t *list) {
    if (list->size == 0)
        return NULL;

    void *data = list->head->data;
    list->head = list->head->next;
    list->size--;

    return data;
}
