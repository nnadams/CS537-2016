#include <stdlib.h>
#include "linkedlist.h"

List *ll_init(int size) {
    List *new = malloc(sizeof(List));
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

void ll_destory(List *list) {
    ListNode *cur = list->head;
    ListNode *tmp;

    while(cur != NULL) {
        tmp = cur->next;
        free(cur->data);
        free(cur);
        cur = tmp;
    }

    free(list);
}

int ll_insert_front(List *list, const void *data) {
    if (list->maxsize > 0 && list->maxsize == list->size)
        return -1;

    ListNode *new = malloc(sizeof(ListNode));
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

int ll_insert_end(List *list, const void *data) {
    if (list->maxsize > 0 && list->maxsize == list->size)
        return -1;

    ListNode *new = malloc(sizeof(ListNode));
    if (new == NULL)
        return -1;

    new->data = (void *)data;

    if (list->size == 0) {
        list->head = new;
        list->tail = new;
        new->next = NULL;
    }
    else {
        new->next = list->tail;
        list->tail = new;
    }

    list->size++;
    return 0;
}

void *ll_pop_front(List *list) {
    if (list->size == 0)
        return NULL;

    void *data = list->head->data;
    list->head = list->head->next;
    list->size--;

    return data;
}
