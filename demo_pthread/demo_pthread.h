//
// Created by longduping on 2019/10/22.
//


#ifndef C_LEARNING_DEMO_PTHREAD_H
#define C_LEARNING_DEMO_PTHREAD_H
#define DEBUG_EVENT

#ifdef DEBUG_EVENT
#define DLOG(fmt, args...)       printf("%s(): " fmt "\n", __func__, ##args)
#else
#define DLOG(fmt, args...)       do{}while(0)
#endif

enum {
    E_PTHREAD_CONDITION_INIT_ERROR = -1,
    E_PTHREAD_MUTEX_INIT_ERROR = -2,
};

void monitor(int cur_size);

void *consumer(void *ctx);

void *producer(void *ctx);

void create_consumer();

void create_producer();

static int ordered(int *pdat, int len);


#endif //C_LEARNING_DEMO_PTHREAD_H
