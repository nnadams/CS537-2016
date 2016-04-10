#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "linkedlist.h"

typedef struct __HashTable {
    int size;
    List **table;
} HashTable;

HashTable *ht_init(int);
void ht_destory(HashTable *);
char *ht_lookup(HashTable const *, char const *);
int ht_insert(HashTable *, char const *);

#endif
