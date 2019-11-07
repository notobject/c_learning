//
// Created by longduping on 2019/10/22.
//

#ifndef SENSOR_EVENT_QUEUE_H
#define SENSOR_EVENT_QUEUE_H

#include <stdlib.h>
#include <time.h>

/* 事件结构 */
struct event {
    int event_id;
    int buf_len;
    char buf[10240];
};

struct entry {
    struct event *e;

    struct entry *prev;
    struct entry *next;
};

struct dequeue {
    struct entry *head;
    struct entry *tail;
    int size;
};

static inline struct entry *dequeue_create_entry(struct event *e) {
    struct entry *entry = (struct entry *) malloc(sizeof(struct entry *));
    entry->prev = entry->next = NULL;
    entry->e = e;
    return entry;
}

static inline struct dequeue *dequeue_create() {
    struct dequeue *dq = (struct dequeue *) malloc(sizeof(struct dequeue *));
    dq->head = dequeue_create_entry(NULL);
    dq->head->prev = dq->head;
    dq->head->next = NULL;
    dq->tail = dq->head;
    dq->size = 0;
    return dq;
}

static inline int dequeue_empty(struct dequeue *dq) {
    return dq->size == 0;
}

static inline int dequeue_size(struct dequeue *dq) {
    return dq->size;
}

static inline void dequeue_push(struct dequeue *dq, struct event *e) {
    if (!dq) {
        return;
    }
    struct entry *entry = dequeue_create_entry(e);
    entry->prev = dq->tail;
    dq->tail->next = entry;

    dq->tail = entry;

    dq->head->prev = dq->tail;
    dq->tail->next = dq->head;

    dq->size++;
}

static inline struct event *dequeue_pop(struct dequeue *dq) {
    if (!dq || dq->size == 0) {
        return NULL;
    }
    /* element to pop */
    struct entry *entry = dq->head->next;
    if (entry == dq->tail) {
        dq->tail = dq->head;
        dq->size--;
        return entry->e;
    }
    dq->head->next = entry->next;
    entry->next->prev = entry->prev;
    dq->size--;
    return entry->e;
}

static inline void dequeue_free(struct dequeue *dq) {
    struct entry *p1 = dq->head;
    struct entry *p2 = p1;
    while (p1->next != dq->head) {
        p2 = p1->next;
        free(p1);
        p1 = p2;
    }
    free(dq);
}

#endif //SENSOR_EVENT_QUEUE_H
