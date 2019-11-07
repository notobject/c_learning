#include "include/mempool.h"
#include "include/mphash.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

#define MEMPOOL_FREELIST_SIZE (5)

#define MEMPOOL_FREELIST_128B (128)
#define MEMPOOL_FREELIST_512B (512)
#define MEMPOOL_FREELIST_1K (1024)
#define MEMPOOL_FREELIST_4K (4096)

typedef struct realdata_s {
    uint16_t offset; // 当前偏移
    uint16_t size; // 包含realdata_t的头大小
    uint16_t prev; // for linked list
    uint16_t next;
    uint16_t realprev; // for memory addr
    uint16_t realnext;
    uint16_t used;
    uint8_t status; // 0--free, 1--busy
} __attribute__((__packed__)) realdata_t;

typedef struct mempool_large_s {
    uint32_t large_flag;
    uint32_t size;
    uint32_t used;
} mempool_large_t;

typedef struct pooldata_s {
    /**
     * freelist[0] 存放小于等于128字节的内存块
     * freelist[1] 存放小于等于512字节的内存块
     * freelist[2] 存放小于等于1K字节的内存块
     * freelist[3] 存放小于等于4K字节的内存块
     * freelist[4] 存放大于4K字节的内存块
     */
    realdata_t *freelist[MEMPOOL_FREELIST_SIZE]; // free memory
    realdata_t *busylist; // used memory
    mempool_t *prev;
    mempool_t *next;
} pooldata_t __attribute__((__aligned__(64)));

struct mempool_s {
    pooldata_t pdata;

    struct { // 不变化的属性
#ifdef __USE_POOLNAME
        char pname[32]; // 仅主内存池使用
#endif
        mp_hash_table_t *ht; // 仅主内存池使用
        mp_hash_table_t *largeht; // 仅主内存池使用
        uint64_t max_memsize: 56, // 仅主内存池使用
                 max_freepool: 8; // 仅主内存池使用
        uint32_t single_max; // 仅主内存池使用
        int32_t max_searchpool; // 仅主内存池使用
        uint32_t pool_size; // 仅主内存池使用
        uint32_t failed_memsize; // 仅主内存池使用
        mempool_judge_alloc_t judge_alloc;
        void *judge_args;
    } config __attribute__((__aligned__(64)));

    struct { // 变化的属性
        mempool_t *freepool; // 仅主内存池使用
        mempool_t *pooltail; // 仅主内存池使用
        uint64_t total_alloced; // 仅主内存池使用
        uint64_t total_leftsize; // 仅主内存池使用
        uint32_t pfree_count; // 仅主内存池使用
        uint32_t leftsize: 24,
                 failed: 8;
    } property __attribute__((__aligned__(64)));
};

#define mempool_get_realdata(pool, offset)\
    (offset) != MEMPOOL_NULL ? (realdata_t *) (((unsigned char *) ((pool) + 1)) + (offset)) : NULL

#define mempool_get_realdata_direct(pool, offset)\
    (realdata_t *) (((unsigned char *) ((pool) + 1)) + (offset))

#define MEMPOOL_NULL (0xffff)
#define MEMPOOL_LARGEFLAG (0xffffffff)

#ifdef __DEBUG_PRINT
static inline void print_mempool(mempool_t *pool)
{
    if (!pool) {
        printf("pool = nil\n");
        return;
    }

    // print freelist
    int32_t i = 0;
    printf("pool = %p, pool_size = %d, left = %d\n", pool, pool->config.pool_size, pool->property.leftsize);
    for (i = 0; i < MEMPOOL_FREELIST_SIZE; i++) {
        realdata_t *datatmp = pool->pdata.freelist[i];
        if (datatmp) {
            int loopcnt = 0;
            printf("freelist[%d] = \n", i);
            while (datatmp) {
                printf("\t%p %hu %hu %hu %hu %hu %hu\n", datatmp, datatmp->offset, datatmp->size,
                        datatmp->prev, datatmp->next, datatmp->realprev, datatmp->realnext);
                if (datatmp->offset > pool->config.pool_size) {
                    pause();
                }
                datatmp = mempool_get_realdata(pool, datatmp->next);
                loopcnt++;
                if (loopcnt >= 100) {
                    pause();
                }
            }
        } else {
            printf("freelist[%d] = nil\n", i);
        }
    }

    // print busylist
    realdata_t *datatmp = pool->pdata.busylist;
    if (datatmp) {
        int loopcnt = 0;
        printf("busylist = \n");
        while (datatmp) {
            printf("\t%p %hu %hu %hu %hu %hu %hu\n", datatmp, datatmp->offset, datatmp->size,
                    datatmp->prev, datatmp->next, datatmp->realprev, datatmp->realnext);
            if (datatmp->offset > pool->config.pool_size) {
                pause();
            }
            datatmp = mempool_get_realdata(pool, datatmp->next);
            loopcnt++;
            if (loopcnt >= 100) {
                pause();
            }
        }
    } else {
        printf("busylist = nil\n");
    }
    printf("\n");
}
#else
#define print_mempool(pool)
#endif // end of __DEBUG_PRINT

static inline uint32_t hash_memaddr(const void *addr)
{
    uintptr_t value = ((uintptr_t) addr);
    return mp_hash(&value, sizeof(uintptr_t));
}

static inline int hash_memcmp(const void *p1, const void *p2)
{
    intptr_t result = (((intptr_t) p1) - ((intptr_t) p2));
    if (result > 0) {
        return 1;
    } else if (result < 0) {
        return -1;
    }

    return 0;
}

static inline void __init_mempool(mempool_t *pool)
{
    int32_t i = 0;
    for (i = 0; i < MEMPOOL_FREELIST_SIZE - 1; i++) {
        pool->pdata.freelist[i] = NULL; // 256B
    }

    pool->pdata.freelist[i] = (realdata_t *) (pool + 1);
    pool->pdata.freelist[i]->offset = 0; // 相对于(freelist+1)，即可用的数据内存的开始
    pool->pdata.freelist[i]->size = pool->config.pool_size - sizeof(mempool_t);
    pool->pdata.freelist[i]->prev = pool->pdata.freelist[i]->next = MEMPOOL_NULL;
    pool->pdata.freelist[i]->realprev = pool->pdata.freelist[i]->realnext = MEMPOOL_NULL;
    pool->pdata.freelist[i]->status = 0;

    pool->pdata.busylist = NULL;

    pool->property.leftsize = pool->pdata.freelist[i]->size;
    pool->property.failed = 0;
}

mempool_t *mempool_init_extra(char *name, uint32_t size, uint64_t maxmem,
        uint32_t single_max, uint32_t max_freepool, int32_t max_searchpool,
        mempool_judge_alloc_t judge_alloc, void *arg)
{
    if (size > 0) {
        size = MEMPOOL_MEMALIGN(size, MEMPOOL_ALIGN_SIZE);
    }
    if (size == 0 || size > MEMPOOL_MAXSIZE) {
        size = mempool_getpagesize();
    }

    mempool_t *pool = (mempool_t *) malloc(size);
    if (!pool) {
        return NULL;
    }
    pool->config.pool_size = size;
    pool->config.failed_memsize = (pool->config.pool_size >> 2);
    pool->config.ht = NULL;
    pool->config.largeht = NULL;

    mempool_set_maxmem(pool, maxmem);
    mempool_set_single_max(pool, single_max);

    pool->config.max_freepool = max_freepool < MEMPOOL_FREEPOOL_MAX ? max_freepool : MEMPOOL_FREEPOOL_MAX;

    pool->config.max_searchpool = max_searchpool;
    pool->config.judge_alloc = judge_alloc;
    pool->config.judge_args = arg;

    if (name) {
#ifdef __USE_POOLNAME
        memcpy(pool->config.pname, name, sizeof(pool->config.pname) - 1);
        pool->config.pname[sizeof(pool->config.pname) - 1] = '\0';
#endif // end of __USE_POOLNAME
    }

    pool->property.pfree_count = 0;
    pool->property.freepool = NULL;

    __init_mempool(pool);
    pool->property.total_alloced = 0;
    pool->property.total_leftsize = pool->property.leftsize;
    pool->pdata.prev = pool->pdata.next = NULL;
    pool->property.pooltail = NULL;

    return pool;
}

void mempool_set_judge_alloc(mempool_t *pool, mempool_judge_alloc_t judge_alloc, void *arg)
{
    if (!pool) {
        return;
    }

    pool->config.judge_alloc = judge_alloc;
    pool->config.judge_args = arg;
}

uint64_t mempool_get_maxmem(mempool_t *pool)
{
    return pool ? pool->config.max_memsize : 0;
}

void mempool_set_maxmem(mempool_t *pool, uint64_t maxmem)
{
    if (!pool) {
        return;
    }

    uint64_t old_maxmem = pool->config.max_memsize;
    pool->config.max_memsize = maxmem;
    if (pool->config.max_memsize > MEMPOOL_MAX_TOTAL) {
        pool->config.max_memsize = MEMPOOL_MAX_TOTAL;
    }
    if (pool->config.max_memsize < pool->config.pool_size) {
        pool->config.max_memsize = pool->config.pool_size;
    }

    if (pool->config.single_max > pool->config.max_memsize) {
        pool->config.single_max = pool->config.max_memsize;
    }

    if (maxmem > 0 && maxmem != old_maxmem) {
        if (pool->config.ht) {
            mp_hash_table_free(pool->config.ht);
        }
        if (pool->config.largeht) {
            mp_hash_table_free(pool->config.largeht);
        }
        uint32_t hash_size = pool->config.max_memsize / pool->config.pool_size + 1;
        pool->config.ht = mp_hash_table_create(hash_memaddr, hash_memcmp, NULL, hash_size);
        mp_hash_table_insert(pool->config.ht, pool);
        pool->config.largeht = mp_hash_table_create(hash_memaddr, hash_memcmp, free, hash_size);
    }
}

void mempool_set_single_max(mempool_t *pool, uint32_t single_max)
{
    if (!pool) {
        return;
    }

    pool->config.single_max = single_max;
    if (pool->config.single_max > pool->config.max_memsize) {
        pool->config.single_max = pool->config.max_memsize;
    }
}

void mempool_set_max_searchpool(mempool_t *pool, int32_t max_searchpool)
{
    if (!pool) {
        return;
    }

    int32_t searchpool = max_searchpool > 0 ? max_searchpool : MEMPOOL_MAX_SEARCHPOOL;
    pool->config.max_searchpool = searchpool;
}

void mempool_destroy(mempool_t *pool)
{
    if (!pool) {
        return;
    }

    if (pool->config.largeht) {
        mp_hash_table_free(pool->config.largeht);
        pool->config.largeht = NULL;
    }

    // 释放freepool
    mempool_t *p = pool, *n = NULL;
    mempool_t *tmp = pool->property.freepool;
    while (tmp) {
        n = tmp->pdata.next;
        assert(tmp->pdata.busylist == NULL);
        free(tmp);
        tmp = n;
    }

    mp_hash_table_free(pool->config.ht);
    pool->config.ht = NULL;

    // 循环释放内存
    for (; p;) {
        print_mempool(p);
        n = p->pdata.next;
        free(p);
        p = n;
    }
}

uint64_t mempool_alloced(mempool_t *pool)
{
    if (!pool) {
        return 0;
    }

    return pool->property.total_alloced + pool->config.pool_size;
}

uint32_t mempool_poolsize(mempool_t *pool)
{
    return pool ? pool->config.pool_size : 0;
}

static inline void *__alloc_large(mempool_t *pool, uint32_t size)
{
    size = MEMPOOL_MEMALIGN(size + sizeof(mempool_large_t), MEMPOOL_ALIGN_SIZE);

    mempool_large_t *large = (mempool_large_t *) malloc(size);
    if (!large) {
        return NULL;
    }

    large->large_flag = MEMPOOL_LARGEFLAG;
    large->size = size;
    large->used = 0;

    if (mp_hash_table_insert(pool->config.largeht, large) == 0) {
        pool->property.total_alloced += size;
    } else {
        free(large);
        return NULL;
    }

    return large + 1;
}

static inline realdata_t *mempool_get_freelist(mempool_t *pool, uint32_t size, int32_t *index)
{
    uint32_t tmp_size = size;
    int32_t tmp_index = -1;
    realdata_t *freelist = NULL;

    do {
        if (tmp_size > MEMPOOL_FREELIST_4K) {
            freelist = pool->pdata.freelist[4];
            tmp_index = 4;
            break;
        } else if (tmp_size > MEMPOOL_FREELIST_1K) {
            freelist = pool->pdata.freelist[3];
            if (freelist == NULL) {
                tmp_size = MEMPOOL_FREELIST_4K + 1;
            } else {
                tmp_index = 3;
            }
        } else if (tmp_size > MEMPOOL_FREELIST_512B) {
            freelist = pool->pdata.freelist[2];
            if (freelist == NULL) {
                tmp_size = MEMPOOL_FREELIST_1K + 1;
            } else {
                tmp_index = 2;
            }
        } else if (tmp_size > MEMPOOL_FREELIST_128B) {
            freelist = pool->pdata.freelist[1];
            if (freelist == NULL) {
                tmp_size = MEMPOOL_FREELIST_512B + 1;
            } else {
                tmp_index = 1;
            }
        } else {
            freelist = pool->pdata.freelist[0];
            if (freelist == NULL) {
                tmp_size = MEMPOOL_FREELIST_128B + 1;
            } else {
                tmp_index = 0;
            }
        }
    } while (!freelist);

    if (index) {
        if (freelist) {
            *index = tmp_index;
        } else {
            *index = -1;
        }
    }

    return freelist;
}

static inline int32_t __get_freeidx(uint32_t size)
{
    int32_t freeidx = 0;

    if (size > MEMPOOL_FREELIST_4K) {
        freeidx = 4;
    } else if (size > MEMPOOL_FREELIST_1K) {
        freeidx = 3;
    } else if (size > MEMPOOL_FREELIST_512B) {
        freeidx = 2;
    } else if (size > MEMPOOL_FREELIST_128B) {
        freeidx = 1;
    }

    return freeidx;
}

static inline void __remove_freenode(mempool_t *pool, realdata_t *freenode, int32_t freeidx)
{
    if (freeidx == -1) {
        freeidx = __get_freeidx(freenode->size);
    }

    realdata_t *next = mempool_get_realdata(pool, freenode->next);
    if (freenode != pool->pdata.freelist[freeidx]) {
        realdata_t *tmp = mempool_get_realdata_direct(pool, freenode->prev);
        tmp->next = freenode->next;
        if (next) {
            next->prev = freenode->prev;
        }
    } else {
        pool->pdata.freelist[freeidx] = next;
        if (pool->pdata.freelist[freeidx]) {
            pool->pdata.freelist[freeidx]->prev = MEMPOOL_NULL;
        }
    }
}

static inline void __add_freenode(mempool_t *pool, realdata_t *freenode, int32_t freeidx)
{
    if (freeidx == -1) {
        freeidx = __get_freeidx(freenode->size);
    }

    if (pool->pdata.freelist[freeidx]) {
        freenode->next = pool->pdata.freelist[freeidx]->offset;
        pool->pdata.freelist[freeidx]->prev = freenode->offset;
    } else {
        freenode->next = MEMPOOL_NULL;
    }
    pool->pdata.freelist[freeidx] = freenode;
    freenode->prev = MEMPOOL_NULL;
}

static inline realdata_t *__split_realdata(mempool_t *pool, realdata_t **realdata, uint32_t size)
{
    realdata_t *rd = *realdata;
    realdata_t *prev = mempool_get_realdata(pool, rd->prev);
    realdata_t *next = mempool_get_realdata(pool, rd->next);
    int32_t freeidx = -1;
    realdata_t *freelist = mempool_get_freelist(pool, (**realdata).size, &freeidx);

    // 确认是否分割realdata
    if (rd->size - size < (sizeof(realdata_t) << 1)) { // 剩余的内存太少，不再分割
        *realdata = NULL;

        // 将realdata从freelist中移除
        if (rd != freelist) {
            if (prev) {
                prev->next = rd->next;
            }
            if (next) {
                next->prev = rd->prev;
            }
        } else {
            pool->pdata.freelist[freeidx] = next;
            if (pool->pdata.freelist[freeidx] != NULL) {
                pool->pdata.freelist[freeidx]->prev = MEMPOOL_NULL;
            }
        }
#ifdef __DEBUG_PRINTALL
        printf("leftdata = nil\n");
#endif
        goto data_return;
    }

    // 分割内存块
    *realdata = (realdata_t *) (((unsigned char *) rd) + size);
    (*realdata)->offset = rd->offset + size;
    (*realdata)->size = rd->size - size;
    (*realdata)->prev = rd->prev;
    (*realdata)->next = rd->next;
    (*realdata)->realprev = rd->offset;
    (*realdata)->realnext = rd->realnext;
    (*realdata)->status = 0;

#ifdef __DEBUG_PRINTALL
    printf("leftdata = %p %hu %hu %hu %hu %hu %hu\n", (*realdata), (*realdata)->offset, (*realdata)->size,
           (*realdata)->prev, (*realdata)->next, (*realdata)->realprev, (*realdata)->realnext);
#endif

    // 更新rd->realnext的realprev
    if (rd->realnext != MEMPOOL_NULL) {
        realdata_t *tmp = mempool_get_realdata_direct(pool, rd->realnext);
        tmp->realprev = (*realdata)->offset;
    }

    rd->size = size;
    rd->realnext = (*realdata)->offset;

    // 更新realdata->next的prev
    if (next) {
        next->prev = (*realdata)->offset;
    }

    if (rd != freelist) { // rd不在头部
        // 更新realdata->prev的next
        prev->next = (*realdata)->offset;
    } else { // rd在头部，即rd == pool->pdata.freelist
        pool->pdata.freelist[freeidx] = *realdata;
    }

    // 判断*realdata的大小，确定*realdata是否放在合适的freelist上
    int32_t freeidx2 = __get_freeidx((*realdata)->size);
    if (freeidx2 != freeidx) {
        // *realdata 从freeidx中移除，并将*realdata添加到freeidx2
        __remove_freenode(pool, *realdata, freeidx);
        __add_freenode(pool, *realdata, freeidx2);
    }

data_return:
    rd->next = rd->prev = MEMPOOL_NULL;
    rd->status = 1;

    return rd;
}

static inline realdata_t *__get_availdata(mempool_t *pool, uint32_t size)
{
    int32_t freeidx = -1;
    realdata_t *tmp = mempool_get_freelist(pool, size, &freeidx);
    realdata_t *avadata = NULL;
    int32_t search_times = 0;

get_availdata:
    avadata = tmp;
    search_times = 0;

    while (tmp) {
        if (tmp->size >= size && avadata->size > tmp->size) {
            avadata = tmp;
        }

        if (search_times++ >= MEMPOOL_MAX_SEARCHINPOOL) {
            break;
        }

        tmp = mempool_get_realdata(pool, tmp->next);
    }

    realdata_t *result = avadata ? (avadata->size >= size ? avadata : NULL) : NULL;
    if (!result && freeidx < MEMPOOL_FREELIST_SIZE - 1 && freeidx >= 0) {
        do {
            tmp = pool->pdata.freelist[++freeidx];
            if (tmp) {
                goto get_availdata;
            }
        } while (freeidx < MEMPOOL_FREELIST_SIZE - 1);
    }

    return result;
}

static inline int32_t __mempool_move2tail(mempool_t *basepool, mempool_t *pool)
{
    if (pool == basepool || pool == basepool->property.pooltail) {
        return -1;
    }

    pool->pdata.prev->pdata.next = pool->pdata.next;
    pool->pdata.next->pdata.prev = pool->pdata.prev;

    pool->pdata.prev = pool->pdata.next = NULL;

    // 将移除的节点添加到链表尾
    basepool->property.pooltail->pdata.next = pool;
    pool->pdata.prev = basepool->property.pooltail;
    basepool->property.pooltail = pool;

    return 0;
}

static inline realdata_t *__alloc_realdata(mempool_t *basepool, mempool_t *pool, uint32_t size)
{
    mempool_t *tmp = pool;
    realdata_t *realdata = NULL;
    realdata_t *avadata = NULL;
    int32_t search_failed = 0;
    int32_t checked_times = 0;

alloc_realdata_again:
    search_failed = 0;

    while (tmp) {
        if (tmp->property.leftsize < size ||
            mempool_get_freelist(tmp, size, NULL) == NULL)
        {
            // 当前内存池内存不足，继续下一块内存
            mempool_t *tmp2 = tmp->pdata.next;
            if (tmp->property.failed < MEMPOOL_FAILED_SKIP) {
                if (tmp->property.leftsize < basepool->config.failed_memsize) {
                    tmp->property.failed++;
                }
            } else {
                __mempool_move2tail(basepool, tmp);
            }

            if (basepool->config.max_searchpool > 0) {
                search_failed++;
                if (search_failed >= basepool->config.max_searchpool) {
                    break;
                }
            }

            tmp = tmp2;
            continue;
        }

        if (tmp->pdata.busylist) { // 内存池已使用
            // 从freelist中查找一块适合分配的内存
            avadata = __get_availdata(tmp, size);
            if (!avadata) {
                // 当前内存池，没有找到合适大小的内存块，继续查找
                if (tmp->property.failed < MEMPOOL_FAILED_SKIP && tmp->property.leftsize < basepool->config.failed_memsize) {
                    tmp->property.failed++;
                }

                if (basepool->config.max_searchpool > 0) {
                    search_failed++;
                    if (search_failed >= basepool->config.max_searchpool) {
                        break;
                    }
                }

                tmp = tmp->pdata.next;
                continue;
            } else {
                tmp->property.failed = 0;
            }

#ifdef __DEBUG_PRINTALL
            print_mempool(tmp);
#endif

            // 获取到合适大小的内存块
            realdata = __split_realdata(tmp, &avadata, size);

            // 将realdata链接到busylist
            realdata->next = tmp->pdata.busylist->offset;
            tmp->pdata.busylist->prev = realdata->offset;
        } else { // 内存池第一次使用
            avadata = tmp->pdata.freelist[MEMPOOL_FREELIST_SIZE - 1];
            realdata = __split_realdata(tmp, &avadata, size);

            if (avadata) {
                int32_t freeidx = __get_freeidx(avadata->size);
                if (freeidx == MEMPOOL_FREELIST_SIZE - 1) {
                    tmp->pdata.freelist[MEMPOOL_FREELIST_SIZE - 1] = avadata;
                } else {
                    tmp->pdata.freelist[MEMPOOL_FREELIST_SIZE - 1] = NULL;
                    tmp->pdata.freelist[freeidx] = avadata;
                }
            } else {
                tmp->pdata.freelist[MEMPOOL_FREELIST_SIZE - 1] = NULL;
            }
        }
        tmp->pdata.busylist = realdata;

        // 更新内存剩余内存的大小
        tmp->property.leftsize -= realdata->size;

        break;
    }

    if (!realdata && checked_times < MEMPOOL_REALLOC_MAXCHKS && tmp) {
        double percent = (basepool->property.total_leftsize * 100.0 / (basepool->property.total_alloced + basepool->config.pool_size));
        if (percent >= MEMPOOL_INPOOL_ALLOC_PERCENT || basepool->property.total_leftsize >= MEMPOOL_INPOOL_SIZE) {
            checked_times = 0;
            tmp = tmp->pdata.next;
            goto alloc_realdata_again;
        } else if (percent >= MEMPOOL_ALLOC_PERCENT) {
            checked_times++;
            tmp = tmp->pdata.next;
            goto alloc_realdata_again;
        }
    }

    if (realdata) {
        basepool->property.total_leftsize -= realdata->size;
#ifdef __DEBUG_PRINTALL
        print_mempool(tmp);
#endif
    }

    return realdata;
}

static inline mempool_t *__get_mempool(mempool_t *pool)
{
    mempool_t *newpool = NULL;
    if (pool->property.freepool) {
        // 优先从freepool中取
        newpool = pool->property.freepool;
        pool->property.freepool = newpool->pdata.next;
        if (pool->property.freepool) {
            pool->property.freepool->pdata.prev = NULL;
        }
        pool->property.pfree_count--;
    } else {
        newpool = mempool_init_extra(NULL, pool->config.pool_size, 0, 0, 0, pool->config.max_searchpool, NULL, NULL);
    }

    if (newpool) {
        if (mp_hash_table_insert(pool->config.ht, newpool) != 0) {
            free(newpool);
            return NULL;
        }
    }

    pool->property.total_alloced += pool->config.pool_size;
    pool->property.total_leftsize += newpool->property.leftsize;

    return newpool;
}

void *palloc(mempool_t *pool, uint32_t size)
{
    if (!pool || (pool->property.total_alloced + pool->config.pool_size) >= pool->config.max_memsize ||
        size > pool->config.single_max || size == 0)
    {
        return NULL;
    }

    if (pool->config.judge_alloc && pool->config.judge_alloc(pool, size, pool->config.judge_args) != 1) {
        return NULL;
    }

    if (size > pool->config.pool_size - sizeof(mempool_t) - sizeof(realdata_t)) {
        return __alloc_large(pool, size);
    }

    realdata_t *realdata = NULL;
    uint32_t realsize = size + sizeof(realdata_t);
    mempool_t *tmp = pool->property.leftsize > realsize ? pool : pool->pdata.next;

ALLOC_AGAIN:
    realdata = __alloc_realdata(pool, tmp, realsize);
    if (!realdata) {
        // 内存超限，就不在分配新的内存池
        if ((pool->property.total_alloced + (pool->config.pool_size << 1)) >= pool->config.max_memsize) {
            return NULL;
        }

        // 当前pool内存不足，添加新池继续分配
        mempool_t *newpool = __get_mempool(pool);
        if (!newpool) {
            return NULL;
        }

        // 新内存池链接到基础池上
        newpool->pdata.next = pool->pdata.next;
        if (pool->pdata.next) {
            pool->pdata.next->pdata.prev = newpool;
        }
        newpool->pdata.prev = pool;
        pool->pdata.next = newpool;

        if (!pool->property.pooltail) {
            pool->property.pooltail = newpool;
        }

        tmp = newpool;
        goto ALLOC_AGAIN;
    }

    realdata->used = 0;

    return realdata + 1;
}

void *pcalloc(mempool_t *pool, uint32_t size)
{
    void *data = palloc(pool, size);

    if (data) {
        memset(data, 0, size);
    }

    return data;
}

static inline mempool_t *__check_realdata_in_pool(mempool_t *pool, realdata_t *realdata)
{
    mempool_t *realpool = (mempool_t *) (((unsigned char *) realdata) - realdata->offset - sizeof(mempool_t));

    // 校验内存池是否存在
    return mp_hash_table_find(pool->config.ht, realpool);
}

static inline mempool_large_t *__check_large_in_pool(mempool_t *pool, mempool_large_t *large)
{
    if (!pool) {
        return (large->large_flag == MEMPOOL_LARGEFLAG &&
                large->size >= large->used) ? large : NULL;
    }

    // 校验内存池是否存在
    mempool_large_t *ptmp = mp_hash_table_find(pool->config.largeht, large);
    return ptmp == large ? large : NULL;
}

void *prealloc(mempool_t *pool, void *addr, uint32_t newsize)
{
    if (pool == NULL) {
        return NULL;
    }

    if (addr == NULL) {
        return palloc(pool, newsize);
    }

    // 简单根据标识，判断内存是否是large内存块
    mempool_large_t *large = (mempool_large_t *) ((unsigned char *) addr - sizeof(mempool_large_t));
    if (large && __check_large_in_pool(pool, large)) {
        if (large->size != newsize) {
            void *newaddr = palloc(pool, newsize);
            if (newaddr) {
                uint32_t size = large->size - sizeof(mempool_large_t);
                pmemcpy_extra(pool, newaddr, addr, size > newsize ? newsize : size);
                mempool_large_t *tmp = (mempool_large_t *) ((unsigned char *) newaddr - sizeof(mempool_large_t));
                tmp->used = large->used;
                pfree(pool, addr);
                return newaddr;
            }
        }

        return NULL;
    }

    // 获取管理节点头
    realdata_t *realdata = (realdata_t *) ((unsigned char *) addr - sizeof(realdata_t));
    if (realdata && __check_realdata_in_pool(pool, realdata) && realdata->size < newsize) {
        void *newaddr = palloc(pool, newsize);
        if (newaddr) {
            uint32_t size = realdata->size - sizeof(realdata_t);
            pmemcpy_extra(pool, newaddr, addr, size > newsize ? newsize : size);
            realdata_t *tmp = (realdata_t *) ((unsigned char *) newaddr - sizeof(realdata_t));
            tmp->used = realdata->used;
            pfree(pool, addr);
            return newaddr;
        }
    }

    return NULL;
}

static inline int __free_large(mempool_t *pool, mempool_large_t *large)
{
    // 判断addr是否large内存块
    mempool_large_t *ltmp = __check_large_in_pool(pool, large);
    if (!ltmp) { // 确认内存块不是大内存块
        return -1;
    }

    pool->property.total_alloced -= large->size;
    mp_hash_table_delete(pool->config.largeht, large);

    return 0;
}

static inline void __mempool_internal_destroy(mempool_t *basepool, mempool_t *pool)
{
    if (pool == basepool->property.pooltail) {
        basepool->property.pooltail = pool->pdata.prev;
        if (basepool->property.pooltail == basepool) {
            basepool->property.pooltail = NULL;
        }
    }

    if (pool->pdata.prev) {
        pool->pdata.prev->pdata.next = pool->pdata.next;
    }
    if (pool->pdata.next) {
        pool->pdata.next->pdata.prev = pool->pdata.prev;
    }

    mp_hash_table_delete(basepool->config.ht, pool);
    basepool->property.total_leftsize -= pool->property.leftsize;

    if (basepool->property.pfree_count < basepool->config.max_freepool) {
        __init_mempool(pool);
        // 添加到freepool中
        pool->pdata.next = basepool->property.freepool;
        if (basepool->property.freepool) {
            basepool->property.freepool->pdata.prev = pool;
        }
        basepool->property.freepool = pool;
        basepool->property.freepool->pdata.prev = NULL;
        basepool->property.pfree_count++;
    } else {
        free(pool);
    }
    basepool->property.total_alloced -= basepool->config.pool_size;
}

static inline void __mempool_move2header(mempool_t *basepool, mempool_t *pool)
{
    if (pool != basepool && pool != basepool->pdata.next) {
        mempool_t *pnext = basepool->pdata.next;
        int32_t moveflag = 0;

        while (pnext->property.failed > (MEMPOOL_FAILED_SKIP >> 1) && pool->property.leftsize > pnext->property.leftsize &&
               pool->property.leftsize >= (pool->config.pool_size >> 1))
        {
            pnext = pnext->pdata.next;
            moveflag++;
            if (moveflag > (MEMPOOL_FAILED_SKIP >> 1) || !pnext) {
                break;
            }
        }

        if (moveflag > (MEMPOOL_FAILED_SKIP >> 1)) {
            if (pool == basepool->property.pooltail) {
                basepool->property.pooltail = pool->pdata.prev;
            }

            mempool_t *pprev = pool->pdata.prev;
            pnext = pool->pdata.next;
            pprev->pdata.next = pnext;
            if (pnext) {
                pnext->pdata.prev = pprev;
            }

            pnext = basepool->pdata.next;
            pool->pdata.next = pnext;
            pnext->pdata.prev = pool;

            basepool->pdata.next = pool;
            pool->pdata.prev = basepool;
        }
    }
}

static inline void __free_realdata(mempool_t *basepool, mempool_t *pool, realdata_t *realdata)
{
    realdata_t *realprev = mempool_get_realdata(pool, realdata->realprev);
    realdata_t *realnext = mempool_get_realdata(pool, realdata->realnext);
    realdata_t *freeheader = realdata;
    uint32_t freesize = realdata->size;

    // realprev从freelist移除
    if (realprev && realprev->status == 0) {
        __remove_freenode(pool, realprev, -1);
        freeheader = realprev;
        freeheader->size += realdata->size;
        freeheader->realnext = realdata->realnext;
    }
    // realnext从freelist移除
    if (realnext && realnext->status == 0) {
        __remove_freenode(pool, realnext, -1);
        freeheader->size += realnext->size;
        freeheader->realnext = realnext->realnext;
    }

    if (freeheader->realnext != MEMPOOL_NULL) {
        realdata_t *tmp = mempool_get_realdata_direct(pool, freeheader->realnext);
        tmp->realprev = freeheader->offset;
    }

    __add_freenode(pool, freeheader, -1);
    freeheader->status = 0;

    pool->property.leftsize += freesize;
    pool->property.failed = 0;

#ifdef __DEBUG_PRINTALL
    print_mempool(pool);
#endif
}

static inline void __pfree_realdata(mempool_t *basepool, mempool_t *pool, realdata_t *realdata)
{
    if (realdata->prev != MEMPOOL_NULL) {
        realdata_t *tmp = mempool_get_realdata_direct(pool, realdata->prev);
        tmp->next = realdata->next;
    } else { // realdata从busylist中移除
        pool->pdata.busylist = mempool_get_realdata(pool, realdata->next);
        if (pool->pdata.busylist) {
            pool->pdata.busylist->prev = MEMPOOL_NULL;
        }
    }
    if (realdata->next != MEMPOOL_NULL) {
        realdata_t *tmp = mempool_get_realdata_direct(pool, realdata->next);
        tmp->prev = realdata->prev;
    }
    realdata->prev = realdata->next = MEMPOOL_NULL;

    basepool->property.total_leftsize += realdata->size;

    if (!pool->pdata.busylist) { // 内存池已空闲
        if (pool != basepool) {
            basepool->property.total_leftsize -= realdata->size;
            __mempool_internal_destroy(basepool, pool); // 释放中间分配的内存池
        } else {
            __init_mempool(pool);
        }
        return;
    }

    if (pool->property.leftsize == 0) { // freelist为空
        int32_t freeidx = __get_freeidx(realdata->size);
        pool->pdata.freelist[freeidx] = realdata;
        pool->pdata.freelist[freeidx]->status = 0;

        pool->property.leftsize += realdata->size;
        pool->property.failed = 0;

        __mempool_move2header(basepool, pool);

        return;
    }

    __free_realdata(basepool, pool, realdata);
}

int32_t pfree(mempool_t *pool, void *data)
{
    if (!pool || !data) {
        return -1;
    }

    // 简单根据标识，判断内存是否是large内存块
    mempool_large_t *large = (mempool_large_t *) ((unsigned char *) data - sizeof(mempool_large_t));
    if (large && __check_large_in_pool(pool, large)) {
        if (__free_large(pool, large) == 0) {
            return 0;
        }
    }

    // 获取管理节点头
    realdata_t *realdata = (realdata_t *) ((unsigned char *) data - sizeof(realdata_t));
    if (!realdata) {
        return -1;
    }

    // 校验内存池是否存在
    mempool_t *realpool = __check_realdata_in_pool(pool, realdata);
    if (!realpool) {
        return -1;
    }

    if (realdata->status == 0) { // 防止重复释放同一个内存块
        return 0;
    }

#ifdef __DEBUG_PRINTALL
    printf("currdata = %p %hu %hu %hu %hu %hu %hu\n", realdata,
           realdata->offset, realdata->size, realdata->prev,
           realdata->next, realdata->realprev, realdata->realnext);
#endif

    // realpool可能已被释放，后续不要操作realpool
    __pfree_realdata(pool, realpool, realdata);
#ifdef __DEBUG_PRINTALL
    print_mempool(pool);
#endif

    return 0;
}

/**
 *  不允许overlap
 */
static inline int32_t __pmemcpy(void *dest, uint32_t dsize, uint32_t used, void *src, uint32_t n)
{
    uint32_t left = dsize - used;
    if (n > left) { // 剩余内存不足
        return -1;
    }

    if ((dest >= src && dest <= src + n) || (src >= dest && src <= dest + dsize)) { // overlap
        return -2;
    }

    memcpy(dest + used, src, n);

    return n;
}

void *pmemcpy_extra(mempool_t *pool, void *dest, void *src, uint32_t n)
{
    int32_t cpylen = 0;

    // 简单根据标识，判断内存是否是large内存块
    mempool_large_t *large = (mempool_large_t *) ((unsigned char *) dest - sizeof(mempool_large_t));
    if (large && __check_large_in_pool(pool, large)) {
        cpylen = __pmemcpy(dest, (large->size - sizeof(mempool_large_t)), large->used, src, n);
        if (cpylen > 0) {
            large->used += cpylen;
        }
        return dest;
    }

    // 获取管理节点头，并且校验内存池是否存在
    realdata_t *realdata = (realdata_t *) ((unsigned char *) dest - sizeof(realdata_t));
    if (!realdata || (pool && !__check_realdata_in_pool(pool, realdata))) {
        return NULL;
    }

    cpylen = __pmemcpy(dest, (realdata->size - sizeof(realdata_t)), realdata->used, src, n);
    if (cpylen > 0) {
        realdata->used += cpylen;
    }

    return dest;
}

int32_t psprintf_extra(mempool_t *pool, void *data, const char *fmt, ...)
{
    if (!data || !fmt) {
        return -1;
    }

    int32_t cpylen = 0;

    // 简单根据标识，判断内存是否是large内存块
    mempool_large_t *large = (mempool_large_t *) ((unsigned char *) data - sizeof(mempool_large_t));
    if (large && __check_large_in_pool(pool, large)) {
        int leftlen = (large->size - sizeof(mempool_large_t) - large->used);
        if (leftlen > 0) {
            va_list ap;
            va_start(ap, fmt);
            cpylen = vsnprintf(data + large->used, leftlen, fmt, ap);
            va_end(ap);
            if (cpylen > 0) {
                large->used += cpylen > leftlen ? leftlen : cpylen;
            }
        }

        return cpylen;
    }

    // 获取管理节点头, 并且校验内存池是否存在
    realdata_t *realdata = (realdata_t *) ((unsigned char *) data - sizeof(realdata_t));
    if (!realdata || (pool && !__check_realdata_in_pool(pool, realdata))) {
        return -1;
    }

    int leftlen = (realdata->size - sizeof(realdata_t) - realdata->used);
    if (leftlen > 0) {
        va_list ap;
        va_start(ap, fmt);
        cpylen = vsnprintf(data + realdata->used, leftlen, fmt, ap);
        va_end(ap);
        if (cpylen > 0) {
            realdata->used += cpylen > leftlen ? leftlen : cpylen;
        }
    }

    return cpylen;
}

void pzero_extra(mempool_t *pool, void *data)
{
    // 简单根据标识，判断内存是否是large内存块
    mempool_large_t *large = (mempool_large_t *) ((unsigned char *) data - sizeof(mempool_large_t));
    if (large && __check_large_in_pool(pool, large)) {
        large->used = 0;
        return;
    }

    // 获取管理节点头
    realdata_t *realdata = (realdata_t *) ((unsigned char *) data - sizeof(realdata_t));
    if (!realdata || (pool && !__check_realdata_in_pool(pool, realdata))) {
        return;
    }

    realdata->used = 0;
}

int32_t pdatasize(mempool_t *pool, void *data)
{
    if (!pool || !data) {
        return -1;
    }

    // 简单根据标识，判断内存是否是large内存块
    mempool_large_t *large = (mempool_large_t *) ((unsigned char *) data - sizeof(mempool_large_t));
    if (large && __check_large_in_pool(pool, large)) {
        return large->size - sizeof(*large);
    }

    // 获取管理节点头
    realdata_t *realdata = (realdata_t *) ((unsigned char *) data - sizeof(realdata_t));
    if (!realdata || !__check_realdata_in_pool(pool, realdata)) {
        return -1;
    }

    return realdata->size - sizeof(*realdata);
}


