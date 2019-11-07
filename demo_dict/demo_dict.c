//
// Created by longduping on 2019/10/30.
//
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "dict.h"

// 一系列回调函数
//typedef struct dictType {
//    unsigned int (*hashFunction)(const void *key);    Hash函数
//    void *(*keyDup)(void *privdata, const void *key); key转换函数, 默认不转换
//    void *(*valDup)(void *privdata, const void *obj); value转换函数，默认不转换
//    int (*keyCompare)(void *privdata, const void *key1, const void *key2);    key 比较函数，默认比较变量地址
//    void (*keyDestructor)(void *privdata, void *key); key内存释放函数
//    void (*valDestructor)(void *privdata, void *obj); value内存释放函数
//} dictType;

unsigned int hashFunction(const void *key)
{
    // MurMur Hash 算法
    return dictGenHashFunction(key, strlen(key));
}

void *keyDup(void *privdata, const void *key)
{
    printf("[%s] key dump -> %s\n", privdata, key);
    return NULL;
}

void *valDup(void *privdata, const void *obj)
{
    printf("[%s] val dump -> %s\n", privdata, obj);
    return NULL;
}

int keyCompare(void *privdata, const void *key1, const void *key2)
{
    return strcmp(key1, key2) == 0;
}

void keyAndValDestructor(void *privdata, void *obj)
{
    free(obj);
}

dictType myDictType = {
        hashFunction,
        NULL,
        NULL,
        keyCompare,
        NULL,
        NULL,
};

int main(int argc, char *argv[])
{
    dict *d = dictCreate(&myDictType, "dict-name");
    dictAdd(d, "hello", "world");

    dictEntry *entry = dictFind(d, "hello");
    printf("key=%s, val=%s, d=%lf, u64=%lu, s64=%lu\n", entry->key, entry->v.val, entry->v.d, entry->v.u64,
           entry->v.s64);

    dictAdd(d, "a", "2");
    dictAdd(d, "b", "world3");
    dictAdd(d, "c", "world4");
    dictAdd(d, "d", "world5");
    dictAdd(d, "e", "world5");
    dictAdd(d, "f", "world5");
    dictDelete(d, "e");

    void *val = dictFetchValue(d, "b");

    printf("val=%s\n", val);

    dictIterator *iter = dictGetIterator(d);
    printf("\n=============\n");
    while ((entry = dictNext(iter)) != NULL) {
        printf("key=%s, val=%s\n", entry->key, entry->v.val);
    }
    printf("size=%lu, sizemask=%lu, used=%lu\n ", d->ht.size, d->ht.sizemask, d->ht.used);

    dictRelease(d);
    return 0;
}