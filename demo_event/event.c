//
// Created by longduping on 2019/10/17.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "event.h"
#include "event_queue.h"
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define THREAD_POOL_MAX_SIZE 32
pthread_cond_t _cond;
pthread_mutex_t _lock;
struct thread threads[THREAD_POOL_MAX_SIZE];
int thread_pool_core_size = 3;
volatile int thread_pool_cur_size = 0;

struct dequeue *events;

void act_event(int eid, char *buf, int buf_len) {
    /* do something */
    printf("[%lu] event(%d), buf=%s, len=%d \n", pthread_self(), eid, buf, buf_len);
}

int add_event(struct event *e) {
    pthread_mutex_lock(&_lock);
    dequeue_push(events, e);
    pthread_cond_broadcast(&_cond);
    pthread_mutex_unlock(&_lock);
}

void *exec_event(void *arg) {
    /* poll task from queue */
    while (1) {
        pthread_mutex_lock(&_lock);
        while (dequeue_empty(events)) {
            pthread_cond_wait(&_cond, &_lock);
        }
        struct event *e = dequeue_pop(events);
        pthread_mutex_unlock(&_lock);
        /* exec event */
        act_event(e->event_id, e->buf, e->buf_len);
    }
}

int event_init(void) {
    /* init event queue */
    events = dequeue_create();

    /* create consumer thread */
    if (pthread_cond_init(&_cond, NULL) != 0) {
        printf("init condition failed: %s ", strerror(errno));
        return E_INIT_ERROR;
    }
    if (pthread_mutex_init(&_lock, NULL) != 0) {
        printf("init mutex failed: %s ", strerror(errno));
        return E_INIT_ERROR;
    }

    while (thread_pool_cur_size < thread_pool_core_size) {
        pthread_create(&threads[thread_pool_cur_size].tid, NULL, exec_event, NULL);
        sprintf(threads[thread_pool_cur_size].name, "%s-%d", "thread", thread_pool_cur_size + 1);
        printf("[%lu] create thread %s[%lu]\n", pthread_self(), threads[thread_pool_cur_size].name,
               threads[thread_pool_cur_size].tid);
        thread_pool_cur_size++;
    }
    printf("[%lu] init event completed: current pool = %d\n", pthread_self(), thread_pool_cur_size);
    return 0;
}

int event_exit() {
    dequeue_free(events);
}

void wait_for_events() {
    int i = 0;
    for (i = 0; i < thread_pool_cur_size; i++) {
        pthread_join(threads[i].tid, NULL);
    }
}

/* for test */
void *test_add_event(void *arg) {
    srand(time(NULL));
    while (1) {
        struct event *e = (struct event *) malloc(sizeof(struct event));
        e->event_id = rand() % 100;
        sprintf(e->buf, "%ld", time(NULL));
        e->buf_len = 11;
        add_event(e);
        usleep(100);
    }
}

/* for test */
int main(int argc, char *argv[]) {
    event_init();
    pthread_t pt;
    pthread_create(&pt, NULL, test_add_event, NULL);
    wait_for_events();
    event_exit();
    exit(0);
}


