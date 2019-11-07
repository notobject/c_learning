//
// Created by longduping on 2019/11/6.
//

#ifndef C_LEARNING_THREAD_POOL_H
#define C_LEARNING_THREAD_POOL_H

#include <pthread.h>
#include <sys/queue.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


#ifdef DEBUG_THREAD_POOL
#define DLOG(fmt, args...)       printf("%s(): " fmt "\n", __func__, ##args)
#else
#define DLOG(fmt, args...)       do{}while(0)
#endif

typedef int (*handler)(void *, size_t);

typedef struct {
    pthread_t t_id;             /* 线程ID */
    time_t create_time;         /* 创建时间 */
} worker_t;

typedef struct job_t {
    void *data;     /* 任务数据 */
    size_t length;  /* 数据长度 */
    TAILQ_ENTRY(job_t) entries;
} job_t;

typedef struct {
    int core_szie;      /* 核心池大小 */
    int max_size;       /* 最大池大小 */
    int current_size;   /* 当前池大小 */
    int max_idle;       /* 核心池外空闲线程的最大生存时间 */
    pthread_cond_t _cond;  /* 任务队列条件变量 */
    pthread_mutex_t _lock; /* 任务队列互斥锁 */
    handler job_handler;   /* 用户定义的处理函数 */
    worker_t *workers;     /* 工作线程 */
    TAILQ_HEAD(tail_queue, job_t) job_queue; /* 任务队列 */
    size_t job_size;        /* 当前任务数量 */
    volatile int shutdown;
} thread_pool_t;

extern thread_pool_t *threadpool_create(int core_size, int max_size, int max_idle, handler job_handler);

extern int threadpool_submit(thread_pool_t *threadpool, void *data, size_t length);

extern void threadpool_wait(thread_pool_t *threadpool);

extern void threadpool_destroy(thread_pool_t *threadpool);


void threadpool_free_job(job_t *job)
{
    if (job) {
        free(job->data);
        free(job);
    }
}

void *threadpool_handler_wrapper(void *p_thread_pool)
{
    thread_pool_t *threadpool = (thread_pool_t *) p_thread_pool;
    int ret;
    if (!threadpool) {
        DLOG("threadpool can not be null");
        return NULL;
    }

    while (!threadpool->shutdown) {
        pthread_mutex_lock(&threadpool->_lock);
        while (TAILQ_EMPTY(&threadpool->job_queue)) {
            pthread_cond_wait(&threadpool->_cond, &threadpool->_lock);
        }
        struct job_t *job = TAILQ_FIRST(&threadpool->job_queue);
        TAILQ_REMOVE(&threadpool->job_queue, job, entries);
        threadpool->job_size--;
        DLOG("[INFO] workers: %d, jobs: %lu", threadpool->current_size, threadpool->job_size);
        pthread_mutex_unlock(&threadpool->_lock);

        /* TODO 考虑针对用户处理完之后的返回值，进行后续的处理，如: 再次入队 */
        ret = threadpool->job_handler(job->data, job->length);

        /* TODO 考虑通过一定的策略来让调用者决定是否释放该内存 */
        threadpool_free_job(job);
    }
}

thread_pool_t *threadpool_create(int core_size, int max_size, int max_idle, handler job_handler)
{
    thread_pool_t *threadpool = (thread_pool_t *) malloc(sizeof(thread_pool_t));
    threadpool->core_szie = core_size;
    threadpool->max_size = max_size;
    threadpool->max_idle = max_idle;
    threadpool->current_size = 0;
    threadpool->job_handler = job_handler;
    threadpool->shutdown = 0;
    threadpool->job_size = 0;
    if (pthread_cond_init(&threadpool->_cond, NULL) != 0) {
        DLOG("init condition failed: %s ", strerror(errno));
    }
    if (pthread_mutex_init(&threadpool->_lock, NULL) != 0) {
        DLOG("init condition failed: %s ", strerror(errno));
    }
    TAILQ_INIT(&threadpool->job_queue);
    threadpool->workers = (worker_t *) malloc(threadpool->max_size * sizeof(worker_t));
    while (threadpool->current_size < threadpool->core_szie) {
        threadpool_incre_worker(threadpool);
    }
    return threadpool;
}

int threadpool_incre_worker(thread_pool_t *threadpool)
{
    if (threadpool->current_size >= threadpool->max_size) {
        return -1;
    }
    if (pthread_create(&(threadpool->workers + threadpool->current_size)->t_id, NULL, threadpool_handler_wrapper,
                       threadpool) != 0) {
        DLOG("create worker failed!");
    }
    (threadpool->workers + threadpool->current_size)->create_time = time(NULL);
    threadpool->current_size++;
    return 0;
}

int threadpool_decre_worker(thread_pool_t *threadpool)
{
    // 减少超过 max_idle的 worker 直到 core_size
    if (threadpool->current_size <= threadpool->core_szie) {
        return -1;
    }
    worker_t *worker_tmp = threadpool->workers;
    while (worker_tmp && threadpool->current_size > threadpool->core_szie) {
        if (time(NULL) - worker_tmp->create_time >= threadpool->max_idle) {
            if (pthread_cancel(worker_tmp->t_id) == 0) {
                threadpool->current_size--;
            }
        }
        worker_tmp++;
    }
    return 0;
}

int threadpool_submit(thread_pool_t *threadpool, void *data, size_t length)
{
    struct job_t *job = (struct job_t *) malloc(sizeof(struct job_t));
    job->data = malloc(length);
    job->length = length;
    memcpy(job->data, data, length);

    pthread_mutex_lock(&threadpool->_lock);
    TAILQ_INSERT_TAIL(&threadpool->job_queue, job, entries);
    threadpool->job_size++;
    /*  TODO 任务拒绝策略 */

    pthread_cond_signal(&threadpool->_cond);
    pthread_mutex_unlock(&threadpool->_lock);

    /*  TODO 线程池扩充策略 */
//    if (threadpool->job_size > threadpool->current_size * 3) {
//        threadpool_incre_worker(threadpool);
//    }
    /*  TODO 线程池缩减策略 */
//    if (threadpool->job_size < threadpool->current_size) {
//        threadpool_decre_worker(threadpool);
//    }
    return 0;
}

void threadpool_wait(thread_pool_t *threadpool)
{
    int i = 0;
    for (i = 0; i < threadpool->current_size; i++) {
        pthread_join((threadpool->workers + i)->t_id, NULL);
    }
}

void threadpool_destroy(thread_pool_t *threadpool)
{
    pthread_mutex_destroy(&threadpool->_lock);
    pthread_cond_destroy(&threadpool->_cond);
    if (threadpool->workers) {
        free(threadpool->workers);
    }
    free(threadpool);
}

#endif //C_LEARNING_THREAD_POOL_H
