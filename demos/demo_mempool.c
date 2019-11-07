//
// Created by longduping on 2019/10/17.
//

#include "demo_mempool.h"


int32_t judge(mempool_t *pool, uint32_t size, void *arg) {
    return 1;
}

// mempool 的用法
void demo_mempool() {
    char *name = "demo_pool";
    uint32_t size = 1024 * 1024;
    uint64_t maxmem = 1024 * 1024 * 10;
    uint32_t single_max = 1024 * 1024 * 2;
    uint32_t max_freepool = 3;
    int32_t max_searchpool = 3;

    mempool_t *pool = mempool_init_extra(name, size, maxmem, single_max, max_freepool, max_searchpool, judge, NULL);

    char hostname[64];
    gethostname(hostname, sizeof(hostname));
    printf("hostname=%s\n", hostname);
    printf("%lu \n", mempool_alloced(pool));
    char *strings = (char *) pcalloc(pool, sizeof(hostname));

    strcpy(strings, hostname);
    printf("%s %p %lu\n", strings, &strings, sizeof(strings));
    printf("%lu \n", mempool_alloced(pool));
    pfree(pool, strings);

    mempool_destroy(pool);
}
