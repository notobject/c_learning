//
// Created by longduping on 2019/11/6.
//

#include "thread_pool.h"
#include <stdio.h>
#include <signal.h>
#include <string.h>

thread_pool_t *threadpool = NULL;

typedef struct {
    char tmp[32];
    int job_id;
} my_job_t;

int my_handler(void *data, size_t length)
{
    my_job_t *job = (my_job_t *) data;
    DLOG("[%lu] work with id=%d, data=%s, length=%lu", pthread_self(), job->job_id, job->tmp, length);
    return 0;
}

void sig_handler(int sig)
{
    if (sig == SIGINT && threadpool != NULL) {
        /* 貌似没起作用呀， volatile关键字没用？ */
        threadpool->shutdown = 1;
    }
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sig_handler);

    threadpool = threadpool_create(3, 16, 60 * 10 /*10 minutes*/, my_handler);

    int i = 1000;
    while (i-- > 0) {
        my_job_t job;
        job.job_id = i;
        sprintf(job.tmp, "data-%d", i);
        threadpool_submit(threadpool, (void *) &job, sizeof(my_job_t));
    }
    threadpool_wait(threadpool);
    threadpool_destroy(threadpool);
    return 0;
}