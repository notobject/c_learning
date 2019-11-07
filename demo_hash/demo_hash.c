//
// Created by longduping on 2019/10/30.
//
// 常见的hash算法

#include <stdio.h>
#include <elf.h>
#include <ctype.h>

/* Thomas Wang's 32 bit Mix Function
 *
 * 功能：计算int类型的hash，返回值也是int类型。
 * 特点：高性能（避免了乘法和除法运算）
 *
 * */
unsigned int IntHash(unsigned int key)
{
    key += ~(key << 15U);
    key ^=  (key >> 10U);
    key +=  (key << 3U);
    key ^=  (key >> 6U);
    key += ~(key << 11U);
    key ^=  (key >> 16U);
    return key;
}

static uint32_t hash_seed = 5381;

/* MurmurHash2, by Austin Appleby
 * Note - This code makes a few assumptions about how your machine behaves -
 * 1. We can read a 4-byte value from any address without crashing
 * 2. sizeof(int) == 4
 *
 * And it has a few limitations -
 *
 * 1. It will not work incrementally.
 * 2. It will not produce the same results on little-endian and big-endian
 *    machines.
 *
 * 特点：高性能，低碰撞
 */
unsigned int MurmurHash2(const void *key, size_t len) {
    /* 'm' and 'r' are mixing constants generated offline.
     They're not really 'magic', they just happen to work well.  */
    uint32_t seed = hash_seed;
    const uint32_t m = 0x5bd1e995;
    const int r = 24;

    /* Initialize the hash to a 'random' value */
    uint32_t h = seed ^ len;

    /* Mix 4 bytes at a time into the hash */
    const unsigned char *data = (const unsigned char *)key;

    while(len >= 4) {
        uint32_t k = *(uint32_t*)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    /* Handle the last few bytes of the input array  */
    switch(len) {
        case 3: h ^= data[2] << 16;
        case 2: h ^= data[1] << 8;
        case 1: h ^= data[0]; h *= m;
    };

    /* Do a few final mixes of the hash to ensure the last few
     * bytes are well-incorporated. */
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return (unsigned int)h;
}

/* DJB Hash
 *
 */
unsigned int DJBHash(char *str)
{
    unsigned int hash = (unsigned int)hash_seed;

    while (*str){
        hash = ((hash << 5) + hash) + (*str++); /* times 33 */
    }
    hash &= ~(1 << 31); /* strip the highest bit */
    return hash;
}

/* DJB Case Hash(不区分大小写)
 *
 */
unsigned int DJBCaseHash(const unsigned char *buf, size_t len) {
    unsigned int hash = (unsigned int)hash_seed;

    while (len--)
        hash = ((hash << 5) + hash) + (tolower(*buf++)); /* hash * 33 + c */
    return hash;
}


/* 32位FNV算法(乘法)
 * 核心思想：将16777619进制转为10进制。
 */
unsigned int M_SHIFT = 0;
unsigned int M_MASK = 0x8765fed1;
unsigned int FNVHash(const unsigned char *key, size_t len)
{
    unsigned int hash = (int) 2166136261L;
    while (len--)
        hash = (hash * 16777619) ^ *(key + len);

    if (M_SHIFT == 0)
        return hash;
    return (hash ^ (hash >> M_SHIFT)) & M_MASK;
}

int main(int argc, char *argv[])
{
    // IntHash
    printf("\nIntHash\n");
    printf("100 -> %d \n", IntHash(99));
    printf("101 -> %d \n", IntHash(100));

    // MurmurHash2
    printf("\nMurmurHash2\n");
    printf("hello -> %d \n", MurmurHash2("hello", 5));
    printf("world -> %d \n", MurmurHash2("world", 5));

    // DJB Hash
    printf("\nDJB Hash\n");
    printf("hello -> %d \n", DJBHash("hello"));
    printf("Hello -> %d \n", DJBHash("Hello"));

    // DJB Case Hash
    printf("\nDJB Case Hash\n");
    printf("hello -> %d \n", DJBCaseHash((const unsigned char *)"hello", 5));
    printf("HeLLo -> %d \n", DJBCaseHash((const unsigned char *)"HeLLo", 5));


    // FNV Hash
    printf("\nFNV Hash\n");
    printf("hello -> %d \n", FNVHash((const unsigned char *)"hello", 5));
    printf("world -> %d \n", FNVHash((const unsigned char *)"world", 5));
    return 0;
}
