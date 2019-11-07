//
// Created by longduping on 2019/10/29.
//
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    double x;
    double y;
} position_t;

// demo 1
typedef struct {
    double x;
    double y;
} __attribute__((packed)) position_packed_t;

typedef struct {
    char ch;
    int a;
} demo_t;

typedef struct {
    char ch;
    int a;
} __attribute__((packed)) demo_packed_t;

// demo 2
struct test
{
    char x1;    // 1字节 + 1填充字节
    short x2;   // 2字节
    float x3;   // 4字节
    char x4;    // 1字节 + 3填充字节
};

int main()
{
    printf("demo  __attribute__\n");

    /* __attribute__((packed))
     * 取消结构或变量在编译过程中的优化对齐（紧凑模式），等价于 __attribute__ ((aligned (1)))。
     */
    printf("%lu, %lu, %lu\n", sizeof(double), sizeof(position_t), sizeof(position_packed_t));
    printf("%lu, %lu, %lu\n", sizeof(char),sizeof(int), sizeof(demo_t));
    printf("%lu, %lu, %lu\n", sizeof(char),sizeof(int), sizeof(demo_packed_t));

    /* __attribute__ ((aligned (SIZE)))
     * 按SIZE大小对齐（GCC默认是4字节，可通过#pragma pack (n) 指定默认对齐字节，取值为[1,4]）
     *
     */

    printf("%lu \n", sizeof(struct test));
    return 0;
}
