//
// Created by longduping on 2019/10/28.
// C语言，原子操作系列
//
#include <stdio.h>
#include <elf.h>

#define MPLOCKED	"lock ; "	   /**< Insert MP lock prefix. */
static inline int
ske_atomic32_cmpset(volatile uint32_t *dst, uint32_t exp, uint32_t src)
{
    uint8_t res;

    __asm__ volatile(
    MPLOCKED
    "cmpxchgl %[src], %[dst];"
    "sete %[res];"
    : [res] "=a" (res),	 /* output */
    [dst] "=m" (*dst)
    : [src] "r" (src),	  /* input */
            "a" (exp),
            "m" (*dst)
    : "memory");		/* no-clobber list */
    return res;
}

int main()
{
    int count = 0;

    // 返回之前的值，并加/减/或/与/异或/非  1
    int ret = __sync_fetch_and_add (&count, 1);
    printf("after __sync_fetch_and_add(): count: %d , ret= %d \n", count, ret);
    ret =  __sync_fetch_and_sub (&count, 1);
    printf("after __sync_fetch_and_sub(): count: %d , ret= %d \n", count, ret);
    ret =  __sync_fetch_and_or (&count, 1);
    printf("after __sync_fetch_and_or(): count: %d , ret= %d \n", count, ret);
    ret =  __sync_fetch_and_and (&count, 1);
    printf("after __sync_fetch_and_and(): count: %d , ret= %d \n", count, ret);
    ret =  __sync_fetch_and_xor (&count, 1);
    printf("after __sync_fetch_and_xor(): count: %d , ret= %d \n", count, ret);
    ret =  __sync_fetch_and_nand (&count,1);
    printf("after __sync_fetch_and_nand(): count: %d , ret= %d \n", count, ret);

    // 加/减/或/与/异或/非 1 之后返回
    ret =  __sync_add_and_fetch (&count, 1);
    printf("after __sync_add_and_fetch(): count: %d , ret= %d \n", count, ret);
    ret =  __sync_sub_and_fetch (&count, 1);
    printf("after __sync_sub_and_fetch(): count: %d , ret= %d \n", count, ret);
    ret =  __sync_or_and_fetch (&count, 1);
    printf("after __sync_or_and_fetch(): count: %d , ret= %d \n", count, ret);
    ret =  __sync_and_and_fetch (&count, 1);
    printf("after __sync_and_and_fetch(): count: %d , ret= %d \n", count, ret);
    ret =  __sync_xor_and_fetch (&count, 1);
    printf("after __sync_xor_and_fetch(): count: %d , ret= %d \n", count, ret);
    ret =  __sync_nand_and_fetch (&count, 1);
    printf("after __sync_nand_and_fetch(): count: %d , ret= %d \n", count, ret);

    /* bool __sync_bool_compare_and_swap (type*ptr, type oldval, type newval, ...)
     * 比较ptr与oldval的值，如果相等则将ptr设为newval，并返回true
     * */
    int b_ret =  __sync_bool_compare_and_swap(&count, -1, 0);
    printf("after __sync_bool_compare_and_swap(): count: %d , b_ret= %s \n", count, b_ret?"True":"False");

    /* type __sync_val_compare_and_swap (type *ptr, type oldval,  type newval, ...)
     * 比较ptr与oldval的值，如果相等则将ptr设为newval，并返回操作之前的值。
     */
    ret = __sync_val_compare_and_swap(&count, 0, 1);
    printf("after __sync_val_compare_and_swap(): count: %d , ret= %d \n", count, ret);

    /* type __sync_lock_test_and_set (type *ptr, type value, ...)
     * 将 ptr 设为 value，并返回之前的值。
     */
    ret = __sync_lock_test_and_set(&count, 0);
    printf("after __sync_lock_test_and_set(): count: %d , ret= %d \n", count, ret);
    return 0;
}
