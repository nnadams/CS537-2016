#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

typedef struct __list_node_t {
    void *data;
    struct __list_node_t *next;
} list_node_t;

typedef struct __list_t {
    int size;
    int maxsize;
    list_node_t *head;
    list_node_t *tail;
} list_t;

list_t *ll_init(int size);
void ll_destory(list_t *list);
int ll_insert_front(list_t *list, const void *data);
int ll_insert_end(list_t *list, const void *data);
void *ll_pop_front(list_t *list);
void *ll_pop_end(list_t *list);

#endif
