//
// Created by longduping on 2019/10/30.
//
// 常见的hash算法

#include <stdio.h>
#include "demo_hash.h"


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
