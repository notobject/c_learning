//
// Created by longduping on 2019/10/22.
//
// 生产者-消费者模型
#include "demo_pthread.h"
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

pthread_cond_t _cond;
pthread_mutex_t _lock;

volatile unsigned int size = 0;

#define MONITOR_LIST_SIZE 10
#define MAX_THREAD_NUM 32
int monitor_list[MONITOR_LIST_SIZE];
int monitor_cur_index = 0;
int thread_num = 0;
volatile int producer_size = 0;
volatile int consumer_size = 0;
pthread_t tid_list[MAX_THREAD_NUM];

typedef enum {
    LIGHT_DATA_DECREASE = -1,   /* 递减 */
    LIGHT_DATA_DISORDER = 0,    /* 无序 */
    LIGHT_DATA_INCREASE,        /* 递增 */
    LIGHT_DATA_STABLE,          /* 稳定 */
} LIGHT_DATA_ENUM;

/* 判断事件数量增长趋势，用于动态调整消费者线程 */
static int ordered(int *pdat, int len)
{
    int t = 0, order = 0;
    if (len == 1)
        return 2;
    t = (pdat[0] < pdat[1]) - (pdat[0] > pdat[1]) + 2 * (pdat[0] == pdat[1]);
    order = ordered(pdat + 1, len - 1);
    if (t * order == 0 || t * order == -1)
        return 0;
    if (t > order)
        t = order;
    return t;
}


void *consumer(void *ctx)
{

    while (true) {
        pthread_mutex_lock(&_lock);
        while (size == 0) {
            pthread_cond_wait(&_cond, &_lock);
        }
        size--;
        DLOG("[%lu] run consumer... %d", pthread_self(), size);
        pthread_mutex_unlock(&_lock);
        sleep(1);
    }
}

void *producer(void *ctx)
{
    while (true) {
        pthread_mutex_lock(&_lock);
        size += 3;
        DLOG("[%lu] run producer... %d", pthread_self(), size);
        pthread_cond_broadcast(&_cond);/* 通知所有等待的消费者 */
        pthread_mutex_unlock(&_lock);
        sleep(1);
        monitor(size);
    }
}

void create_consumer()
{
    if (thread_num == MAX_THREAD_NUM) {
        DLOG("%s", "too many thread!");
        return;
    }
    if (pthread_create(&tid_list[thread_num++], NULL, consumer, NULL) != 0) {
        DLOG("create thread-consumer failed: %s", strerror(errno));
    }
    consumer_size++;
    DLOG("[%lu] create thread-consumer success: %lu", pthread_self(), tid_list[thread_num - 1]);
}

void create_producer()
{
    if (thread_num == MAX_THREAD_NUM) {
        DLOG("%s", "too many thread!");
        return;
    }
    if (pthread_create(&tid_list[thread_num++], NULL, producer, NULL) != 0) {
        DLOG("create thread-producer failed: %s", strerror(errno));
    }
    producer_size++;
    DLOG("[%lu] create thread-producer success: %lu", pthread_self(), tid_list[thread_num - 1]);
}


void monitor(int cur_size)
{
    DLOG("current_size: %d, consumer: %d, producer: %d", cur_size, consumer_size, producer_size);

    monitor_list[monitor_cur_index++ % MONITOR_LIST_SIZE] = cur_size;
    if (monitor_cur_index == MONITOR_LIST_SIZE) {
        monitor_cur_index = 0;
        int t = ordered(monitor_list, MONITOR_LIST_SIZE);
        switch (t) {
            case LIGHT_DATA_DECREASE:
                DLOG("DECREASE");
                break;
            case LIGHT_DATA_DISORDER:
                DLOG("DISORDER");
                create_consumer();
                break;
            case LIGHT_DATA_INCREASE:
                DLOG("INCREASE");
                create_consumer();
                break;
            case LIGHT_DATA_STABLE:
                DLOG("STABLE");
                break;
            default:
                DLOG("UNKNOWN");
                break;
        }
    }
}


int main(int argc, char *argv[])
{

    int i, j;

    if (pthread_cond_init(&_cond, NULL) != 0) {
        DLOG("init condition failed: %s ", strerror(errno));
    }
    if (pthread_mutex_init(&_lock, NULL)) {
        DLOG("init mutex failed: %s ", strerror(errno));
    }

    create_producer();
    create_producer();
    create_consumer();

    for (j = 0; j < thread_num; ++j) {
        pthread_join(tid_list[j], NULL);
    }
    return 0;
}