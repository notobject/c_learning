//
// Created by longduping on 2019/10/29.
//
#include <stdio.h>


#define IS_POWER_OF_TWO(x) (((x) & (x) - 1) == 0) /* x是不是2的幂 */

/**
 * x的二进制表示中，最左边1的位置
 * @param x
 * @return position
 */
static inline unsigned int first_position_of_bit_one(unsigned int x)
{
    unsigned int r;
    __asm__ volatile("bsrl %1,%0\n\t"
                     "jnz 1f\n\t"
                     "movl $-1,%0\n"
                     "1:" : "=r" (r) : "rm" (x));
    return r + 1;
}

/**
 * 如果size是2的幂，返回size。否则将size向上转为2的幂之后返回
 * @return size
 */
static inline unsigned int roundup_power_of_two(unsigned int size)
{
    return __glibc_likely(IS_POWER_OF_TWO(size)) ? size : 1U << first_position_of_bit_one(size);
}

int main()
{
    unsigned int size = roundup_power_of_two(14);
    printf("size = %d\n", size);
    unsigned int mask = size - 1;
    printf("mask = %d\n", size - 1);
    int i = 0;
    for (i = 0; i < 20; i++)
        printf("%d\n", i & mask);

    int index = 0;
    int total = 3;
    int flag = 0;
    for (i = 0; i < 100; i++) {
        index = i % total;
        flag |= (1 << index);
        printf("\tindex:%d, total:%d, flag:%d, (1 << index):%d, (1 << total) - 1:%d\n", index, total, flag,
               (1 << index), (1 << total) - 1);
        if (flag == (1 << total) - 1) {
            printf("ok\n");
            flag = 0;
        }

    }
    return 0;
}