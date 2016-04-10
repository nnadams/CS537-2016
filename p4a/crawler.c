#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <pthread.h>

#include "linkedlist.h"
#include "hashtable.h"

#define MAX_LEN 255
#define MAX_WORKERS 100

#define error(M, ...) fprintf(stderr, M, ##__VA_ARGS__);
#define die(M, ...) { error(M, ##__VA_ARGS__); exit(-1); }

// Link Queue;
pthread_cond_t lqueue_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t lqueue_fill = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lm = PTHREAD_MUTEX_INITIALIZER;
List *link_queue;
HashTable *visited_links;

// Page Queue
pthread_cond_t pcv = PTHREAD_COND_INITIALIZER;
pthread_mutex_t pm = PTHREAD_MUTEX_INITIALIZER;
List *page_queue;

// Hash
pthread_mutex_t hm = PTHREAD_MUTEX_INITIALIZER;

// Main
pthread_cond_t mcv = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mm = PTHREAD_MUTEX_INITIALIZER;

void *fetcher(void *arg) {
    char *data;
    while (1) {
        if (link_queue->size > 0) {
            data = (char *)ll_pop_front(link_queue);
            printf("%s fetched!\n", data);
            ll_insert_front(page_queue, data);
        }
    }
}

void *parser(void *arg) {
    while (1) {
        if (page_queue->size > 0) {
            printf("%s processed!\n", (char *)ll_pop_front(page_queue));
        }
    }
}

int crawl(char *start_url,
	  int download_workers,
	  int parse_workers,
	  int queue_size,
	  char * (*fetch_fn)(char *url),
	  void (*edge_fn)(char *from, char *to)) {

    char *url = strndup(start_url, MAX_LEN+1);
    if (strnlen(start_url, MAX_LEN+1) > MAX_LEN)
        die("Bad start_url\n");
    if (download_workers < 1 || parse_workers < 1
            || download_workers > MAX_WORKERS
            || parse_workers > MAX_WORKERS)
        die("Bad worker count\n");
    if (queue_size < 1)
        die("Bad queue size\n");

    link_queue = ll_init(queue_size);
    page_queue = ll_init(0);
    visited_links = ht_init(queue_size*queue_size);

    ll_insert_front(link_queue, url);

    int i;
    pthread_t fids[MAX_WORKERS], pids[MAX_WORKERS];

    for (i = 0; i < download_workers; i++)
        pthread_create(&fids[i], NULL, (void *) &fetcher, NULL);
    for (i = 0; i < parse_workers; i++)
        pthread_create(&pids[i], NULL, (void *) &parser, NULL);

    for (i = 0; i < download_workers; i++)
        pthread_join(fids[i], NULL);
    for (i = 0; i < parse_workers; i++)
        pthread_join(pids[i], NULL);

    return 0;
}
