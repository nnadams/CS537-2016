#include "hashtable.h"

// Taken from Wikipedia
// https://en.wikipedia.org/wiki/Fletcher's_checksum
static uint32_t fletcher32(char const *data, size_t words) {
    uint32_t sum1 = 0xffff, sum2 = 0xffff;
    size_t tlen;

    while (words) {
            tlen = words >= 359 ? 359 : words;
            words -= tlen;
            do {
                    sum2 += sum1 += *data++;
            } while (--tlen);
            sum1 = (sum1 & 0xffff) + (sum1 >> 16);
            sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    }
    /* Second reduction step to reduce sums to 16 bits */
    sum1 = (sum1 & 0xffff) + (sum1 >> 16);
    sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    return sum2 << 16 | sum1;
}

hashtable_t *ht_init(int size) {
    hashtable_t *new;
    int i;

    if (size < 1) return NULL;
    if ((new = malloc(sizeof(hashtable_t))) == NULL) return NULL;
    if ((new->table = malloc(sizeof(list_t*) * size)) == NULL) return NULL;

    for(i = 0; i < size; i++)
        new->table[i] = ll_init(0);

    new->size = size;
    return new;
}

void ht_destory(hashtable_t *ht) {
    int i;

    if (ht == NULL) return;

    for(i = 0; i < ht->size; i++)
        ll_destory(ht->table[i]);

    free(ht->table);
    free(ht);
}

char *ht_lookup(hashtable_t const *ht, char const *str) {
    int len = strnlen(str, 1024);
    if (len > 1023) return NULL;
    if (ht == NULL) return NULL;

    uint32_t val = fletcher32(str, len);
    val = val % ht->size;

    list_node_t *cur = ht->table[val]->head;
    for (; cur != NULL; cur = cur->next) {
        if (strncmp(str, (char *)cur->data, len) == 0)
            return (char *)cur->data;
    }

    return NULL;
}

int ht_insert(hashtable_t *ht, char const *str) {
    int len = strnlen(str, 1024);
    if (len > 1023) return -1;
    if (ht == NULL) return -1;

    uint32_t val = fletcher32(str, len);
    val = val % ht->size;

    if (ht_lookup(ht, str) != NULL) return 1;
    return ll_insert_front(ht->table[val], (void *)strndup(str, len));
}
