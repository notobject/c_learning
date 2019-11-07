#include <stdio.h>
#include <stdbool.h>

void demo_bool()
{
    // C语言中的布尔类型, 任何非0值都会被转为1
    _Bool is_ok = 100;

    // C99提供的stdbool.h库，可以使用bool代替_Bool，用true,false分别表示1，0。从而让代码与C++兼容
    bool cpp_like_bool = true;
    printf("_Bool = %d \n", is_ok);
    printf("cpp_like_bool= %d \n", cpp_like_bool);
}

void demo_rand()
{
    // 随机数,stdlib.h
    srand((unsigned int) time(NULL));
    int n = 5;
    while (n-- > 0) {
        printf("%d \n", rand() % 100);
    }
}

void demo_malloc()
{
    // malloc
    void *p = malloc(1); // 分配1 byte大小的内存空间，将内存地址赋值给c
    // 由于malloc返回的是通用指针类型 void，这里需要转换成实际的指针类型
    *((char *) p) = 'A';
    printf("c的地址：%p , c的值:%c \n", p, *((char *) p));
    // 释放p所执行的内存
    free(p);
    *((char *) p) = 'B'; // 释放内存后仍然可以对该内存区域操作，但是不安全，随时会被覆盖
    printf("c的地址：%p , c的值:%c \n", p, *((char *) p));

    // calloc ，与malloc的区别: calloc分配完成后会将内存块中所有的位 置为：0
    void *ptr = calloc(2, 1);// 申请分配2个单元，其中每个单元4字节的内存空间（共2 * 4 = 8 字节）。
    *((char *) ptr) = 'a';
    *((char *) ptr + 1) = 'b'; // 加1表示 偏移一个单位大小的字节(该单位取决于被转换的指针类型 所执向的数据结构大小，int=4, char=1...)
    printf("ptr的地址：%p , ptr的值:%d , %d \n", ptr, *((char *) ptr), *((char *) ptr + 1));
    free(ptr);
}


struct command {
    char *name;
    int enable;

    void (*func)();
};

struct command cmds[] = {
        {"demo_bool",   1, demo_bool},
        {"demo_rand",   1, demo_rand},
        {"demo_malloc", 1, demo_malloc},
        /* 命令模式 add more... */
        {NULL,          0, NULL},
};

int main()
{
    int i = 0;

    while (cmds[i].func) {
        if (!cmds[i].enable) continue;
        printf("==========%s()============ \n", cmds[i].name);
        cmds[i].func();
        printf("\n");
        i++;
    }
    printf("\n\n==========all done.================ \n");
    return 0;
}