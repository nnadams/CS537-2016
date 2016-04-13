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
int work;

typedef struct __page_t {
    char *name;
    char *text;
} page_t;

typedef char* fetch_fn(char*);
typedef void edge_fn(char*, char*);

void *fetcher(void *arg) {
    page_t *newpage;
    char *data;
    while (1) {
        pthread_mutex_lock(&lm);
        while (link_queue->size == 0)
            pthread_cond_wait(&lqueue_fill, &lm);
        data = (char *)ll_pop_front(link_queue);
        pthread_cond_signal(&lqueue_empty);
        pthread_mutex_unlock(&lm);

        newpage = malloc(sizeof(page_t));
        newpage->name = data;
        data = ((fetch_fn *)arg)(data);
        newpage->text = data;

        pthread_mutex_lock(&pm);
        ll_insert_end(page_queue, (void *)newpage);
        pthread_cond_signal(&pcv);
        pthread_mutex_unlock(&pm);
    }
}

void *parser(void *arg) {
    page_t *page;
    int rc;
    char *data, *link, *token, *save;
    while (1) {
        pthread_mutex_lock(&pm);
        while (page_queue->size == 0)
            pthread_cond_wait(&pcv, &pm);
        page = (page_t *)ll_pop_front(page_queue);
        pthread_mutex_unlock(&pm);

        data = page->text;
        while ((token = strtok_r(data, " \n", &data))) {
            save = token;
            token = strtok_r(token, ":", &save);
            link = strtok_r(NULL, ":", &save);

            if (token == NULL || link == NULL) continue;
            if (strncmp(token, "link", 5) != 0) continue;

            ((edge_fn *)arg)(page->name, link);

            pthread_mutex_lock(&hm);
            rc = ht_insert(visited_links, link);
            pthread_mutex_unlock(&hm);
            if (rc == 1) continue;

            pthread_mutex_lock(&lm);
            while (link_queue->size == link_queue->maxsize)
                pthread_cond_wait(&lqueue_empty, &lm);
            ll_insert_end(link_queue, (void *)link);
            pthread_mutex_lock(&mm);
            work++;
            pthread_mutex_unlock(&mm);
            pthread_cond_signal(&lqueue_fill);
            pthread_mutex_unlock(&lm);
        }

        pthread_mutex_lock(&mm);
        work--;
        pthread_mutex_unlock(&mm);
        if (!work) pthread_cond_signal(&mcv);
    }
}

int crawl(char *start_url,
	  int download_workers,
	  int parse_workers,
	  int queue_size,
	  char * (*fetch_fn)(char *url),
	  void (*edge_fn)(char *from, char *to)) {

    if (strnlen(start_url, MAX_LEN+1) > MAX_LEN)
        die("Bad start_url\n");

    if (queue_size < 1)
        die("Bad queue size\n");

    if (download_workers < 1 || parse_workers < 1
            || download_workers > MAX_WORKERS
            || parse_workers > MAX_WORKERS)
        die("Bad worker count\n");

    link_queue = ll_init(queue_size);
    page_queue = ll_init(0);
    visited_links = ht_init((queue_size+1)*queue_size);

    ll_insert_front(link_queue, start_url);
    ht_insert(visited_links, start_url);
    work = 1;

    int i;
    pthread_t fids[MAX_WORKERS], pids[MAX_WORKERS];

    for (i = 0; i < download_workers; i++)
        pthread_create(&fids[i], NULL, (void *) &fetcher, (void *)fetch_fn);
    for (i = 0; i < parse_workers; i++)
        pthread_create(&pids[i], NULL, (void *) &parser, (void *)edge_fn);

    while (work)
        pthread_cond_wait(&mcv, &mm);

    /*for (i = 0; i < download_workers; i++)
        pthread_join(fids[i], NULL);
    for (i = 0; i < parse_workers; i++)
        pthread_join(pids[i], NULL);*/

    return 0;
}
