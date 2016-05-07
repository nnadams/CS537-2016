#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "linkedlist.h"

typedef struct __HashTable {
    int size;
    list_t **table;
} hashtable_t;

hashtable_t *ht_init(int);
void ht_destory(hashtable_t *);
char *ht_lookup(hashtable_t const *, char const *);
int ht_insert(hashtable_t *, char const *);

#endif
