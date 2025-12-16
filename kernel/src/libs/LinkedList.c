#include "LinkedList.h"
#include "../mem/heap.h"

void ll_init(linkedlist_t *list) {
    list->head = 0;
    list->tail = 0;
    list->size = 0;
}

int ll_push_back(linkedlist_t *list, void *data) {
    ll_node_t *n = (ll_node_t *)ll_alloc(sizeof(ll_node_t));
    if (!n) return -1;

    n->data = data;
    n->next = 0;

    if (!list->tail) {
        list->head = n;
        list->tail = n;
    } else {
        list->tail->next = n;
        list->tail = n;
    }

    list->size++;
    return 0;
}

int ll_push_front(linkedlist_t *list, void *data) {
    ll_node_t *n = (ll_node_t *)ll_alloc(sizeof(ll_node_t));
    if (!n) return -1;

    n->data = data;
    n->next = list->head;
    list->head = n;

    if (!list->tail)
        list->tail = n;

    list->size++;
    return 0;
}

void *ll_pop_front(linkedlist_t *list) {
    if (!list->head) return 0;

    ll_node_t *n = list->head;
    void *data = n->data;

    list->head = n->next;
    if (!list->head)
        list->tail = 0;

    ll_free(n);
    list->size--;
    return data;
}

void *ll_pop_back(linkedlist_t *list) {
    if (!list->head) return 0;

    if (list->head == list->tail) {
        void *data = list->head->data;
        ll_free(list->head);
        list->head = 0;
        list->tail = 0;
        list->size = 0;
        return data;
    }

    ll_node_t *cur = list->head;
    while (cur->next != list->tail)
        cur = cur->next;

    void *data = list->tail->data;
    ll_free(list->tail);
    cur->next = 0;
    list->tail = cur;
    list->size--;
    return data;
}

void *ll_get(linkedlist_t *list, size_t index) {
    if (index >= list->size) return 0;

    ll_node_t *cur = list->head;
    while (index--)
        cur = cur->next;

    return cur->data;
}

void ll_clear(linkedlist_t *list) {
    while (list->head)
        ll_pop_front(list);
}

void *ll_alloc(size_t size) {
    return malloc(size);
}

void ll_free(void *ptr) {
    free(ptr);
}
