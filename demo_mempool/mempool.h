#ifndef __MEMPOLL_H_H_H__
#define __MEMPOLL_H_H_H__

#include <unistd.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MEMPOOL_MAXSIZE (65536) // 64K
#define MEMPOOL_DEFAULT_POOLSIZE (4096) // 4K

#define MEMPOOL_ALIGN_SIZE (8)
#define MEMPOOL_MEMALIGN(size, align) (((size) + ((align) - 1)) & ~((align) - 1))

#define MEMPOOL_FAILED_SKIP (8)

// 内存最少允许的内存(1M)
#define MEMPOOL_MIN_TOTAL (1048576)
// 内存最大允许的内存(32G)
#define MEMPOOL_MAX_TOTAL (34359738368)

#define MEMPOOL_SINGLE_MAX (268435456)

#define MEMPOOL_FREE_POOLCNT (32)
#define MEMPOOL_FREEPOOL_MAX (256)

// 内存池之前搜索的次数
#define MEMPOOL_MAX_SEARCHPOOL (-1)
// 内存池内搜索内存块的次数
#define MEMPOOL_MAX_SEARCHINPOOL (8)

// 分配内存块失败的重试次数
#define MEMPOOL_REALLOC_MAXCHKS (8)
// 当内存池的空闲内存大于比例值时，尽量在池内分配
//#define MEMPOOL_INPOOL_SIZE (536870912)
#define MEMPOOL_INPOOL_SIZE (268435456)
// 当内存池的空闲内存比例大于此值时，尽量在池内分配
#define MEMPOOL_INPOOL_ALLOC_PERCENT (20)
// 当内存池的空闲内存比例大于此值时，才进行重试
#define MEMPOOL_ALLOC_PERCENT (10)

typedef struct mempool_s mempool_t;

typedef int32_t (*mempool_judge_alloc_t)(mempool_t *pool, uint32_t size, void *arg);

/**
 * init memory pool
 *
 * name: pool name
 * size: single pool size
 * maxmem: total pools(included linked pools) size, max 4G
 * single_max: max alloced size for once, default 256M.
 * max_freepool: max free pools in the pool, default 32, max 256.
 * max_searchpool: search depth for alloc memory. If max_searchpool > 0, if fail to alloc memory from one pool,
 *                 record one failed. If failed times up to max_searchpool, then stop searching, and alloc new
 *                 pool to alloc memory.
 * judge_alloc: user-defined function, to judge whether alloc memory.
 *              if judge_alloc return 1, then alloc memory, otherwise, don't.
 * arg: paramters for judge_alloc function.
 */
mempool_t *mempool_init_extra(char *name, uint32_t size, uint64_t maxmem,
        uint32_t single_max, uint32_t max_freepool, int32_t max_searchpool,
        mempool_judge_alloc_t judge_alloc, void *arg);

#ifdef __USE_POOLNAME
#define mempool_init(name, size) mempool_init_extra((name), (size),\
        MEMPOOL_MAX_TOTAL, MEMPOOL_SINGLE_MAX, MEMPOOL_FREE_POOLCNT, MEMPOOL_MAX_SEARCHPOOL, NULL, NULL)
#else
#define mempool_init(size) mempool_init_extra(NULL, (size),\
        MEMPOOL_MAX_TOTAL, MEMPOOL_SINGLE_MAX, MEMPOOL_FREE_POOLCNT, MEMPOOL_MAX_SEARCHPOOL, NULL, NULL)
#endif

/**
 * set judge_alloc function for one pool.
 *
 * judge_alloc: user-defined function, to judge whether alloc memory.
 *              if judge_alloc return 1, then alloc memory, otherwise, don't.
 * arg: paramters for judge_alloc function.
 */
void mempool_set_judge_alloc(mempool_t *pool, mempool_judge_alloc_t judge_alloc, void *arg);

uint64_t mempool_get_maxmem(mempool_t *pool);

void mempool_set_maxmem(mempool_t *pool, uint64_t maxmem);

void mempool_set_single_max(mempool_t *pool, uint32_t single_max);

void mempool_set_max_searchpool(mempool_t *pool, int32_t max_searchpool);

void mempool_destroy(mempool_t *pool);

/**
 * get all alloced memory in the pool.
 */
uint64_t mempool_alloced(mempool_t *pool);

/**
 * get poolsize
 */
uint32_t mempool_poolsize(mempool_t *pool);

void *palloc(mempool_t *pool, uint32_t size);

void *pcalloc(mempool_t *pool, uint32_t size);

void *prealloc(mempool_t *pool, void *addr, uint32_t newsize);

int32_t pfree(mempool_t *pool, void *addr);

void *pmemcpy_extra(mempool_t *pool, void *dest, void *src, uint32_t n);
#define pmemcpy(dest, src, n) pmemcpy_extra(NULL, (dest), (src), (n))

int32_t psprintf_extra(mempool_t *pool, void *data, const char *fmt, ...);
#define psprintf(data, fmt, ...) psprintf_extra(NULL, (data), (fmt), ##__VA_ARGS__)

void pzero_extra(mempool_t *pool, void *data);
#define pzero(data) pzero_extra(NULL, (data))

int32_t pdatasize(mempool_t *pool, void *data);

#if HAS_GETPAGESIZE
static inline uint32_t mempool_getpagesize()
{
    int pagesize = getpagesize();
    if (pagesize <= MEMPOOL_DEFAULT_POOLSIZE) {
        pagesize = MEMPOOL_DEFAULT_POOLSIZE;
    } else if (pagesize > MEMPOOL_MAXSIZE) {
        pagesize = MEMPOOL_MAXSIZE;
    }

    return pagesize;
}
#else
static inline uint32_t mempool_getpagesize()
{
    return MEMPOOL_DEFAULT_POOLSIZE;
}
#endif

#ifdef __cplusplus
}
#endif

#endif // end of __MEMPOLL_H_H_H__


