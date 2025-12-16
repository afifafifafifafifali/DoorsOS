#pragma once

#include <stddef.h>

void *ll_alloc(size_t size);
void  ll_free(void *ptr);

typedef struct ll_node {
    void *data;
    struct ll_node *next;
} ll_node_t;

typedef struct {
    ll_node_t *head;
    ll_node_t *tail;
    size_t size;
} linkedlist_t;

void ll_init(linkedlist_t *list);
int  ll_push_back(linkedlist_t *list, void *data);
int  ll_push_front(linkedlist_t *list, void *data);
void *ll_pop_front(linkedlist_t *list);
void *ll_pop_back(linkedlist_t *list);
void *ll_get(linkedlist_t *list, size_t index);
void ll_clear(linkedlist_t *list);


