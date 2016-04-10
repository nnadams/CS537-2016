#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

typedef struct __ListNode {
    void *data;
    struct __ListNode *next;
} ListNode;

typedef struct __List {
    int size;
    int maxsize;
    ListNode *head;
    ListNode *tail;
} List;

List *ll_init(int size);
void ll_destory(List *list);
int ll_insert_front(List *list, const void *data);
int ll_insert_end(List *list, const void *data);
void *ll_pop_front(List *list);
void *ll_pop_end(List *list);

#endif
