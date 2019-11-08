#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <setjmp.h>

/*                C程序 内存区域
 * +----------------------------------------+
 * |               env and argv             |       +-----------+ ——> %ebp (基地址指针)
 * +----------------------------------------+       |           |
 * |                                        |       |  函数栈帧  |
 * |                栈空间 ↓                 |       |           |
 * |                                        |       +------------ ——> %esp (栈指针)
 * +------------共享库和内存映射区------------+
 * |                                        |
 * |                堆空间 ↑                 |
 * |                                        |
 * +----------------------------------------+
 * |            未初始化数据区                |
 * +----------------------------------------+
 * |            初始化数据区（自动变量）       |
 * +----------------------------------------+
 * |            程序正文（代码段）            |
 * +----------------------------------------+
 */
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
    /* malloc */
    void *p = malloc(1); // 分配1 byte大小的内存空间，将内存地址赋值给c
    // 由于malloc返回的是通用指针类型 void，这里需要转换成实际的指针类型
    *((char *) p) = 'A';
    printf("c的地址：%p , c的值:%c \n", p, *((char *) p));
    // 释放p所执行的内存
    free(p);
    *((char *) p) = 'B'; // 释放内存后仍然可以对该内存区域操作，但是不安全，随时会被覆盖
    printf("c的地址：%p , c的值:%c \n", p, *((char *) p));

    /* calloc  */
    // 与malloc的区别: calloc分配完成后会将内存块中所有的位 置为：0
    void *ptr = calloc(2, 1);// 申请分配2个单元，其中每个单元4字节的内存空间（共2 * 4 = 8 字节）。
    *((char *) ptr) = 'a';
    *((char *) ptr + 1) = 'b'; // 加1表示 偏移一个单位大小的字节(该单位大小取决于被转换的指针 所执向的数据结构大小，int=4, char=1...)
    printf("ptr的地址：%p , ptr的值:%d , %d \n", ptr, *((char *) ptr), *((char *) ptr + 1));
    free(ptr);

    /* realloc 
     *
     * 增加或减少之前分配区的长度（可能或将原有数据整体移动到另一个地方，新增部分的初始值则不确定）
     * */
    void *p_old = malloc(1);
    printf("old=%s\n", p_old);
    // 重新分配两个字节空间,当第一个参数为NULL时，realloc的行为与malloc一致
    void *p_new = realloc(p_old, 2);
    printf("new=%s\n", p_new);
    free(p_new);

    /* alloca
     *
     * 在栈上分配空间，（程序调用结束后会自动释放所分配的空间）。需要注意的是：某些系统不支持程序被加载后动态调整栈大小。
     * */
    void *pstack = alloca(1024);
    printf("ptr (stack): %p\n", pstack);
}

void demo_sysenv()
{
    /* 获取系统的环境变量 */
    char *value = getenv("PATH");
    printf("PATH=%s\n", value);

    /* 设置系统环境变量 */
    int res = putenv("DEMO_ENV=it's work!");
    printf("putenv   status: %d ,DEMO_ENV=%s\n", res, getenv("DEMO_ENV"));

    // 设置系统环境变量，第三个参数: 非0时若该变量存在，则先删除已有定义再设置。为0时若该变量存在，则什么也不做。
    res = setenv("DEMO_ENV", "yes, it's work!", 1);
    printf("setenv   status: %d ,DEMO_ENV=%s\n", res, getenv("DEMO_ENV"));

    // 删除系统环境变量
    res = unsetenv("DEMO_ENV");
    printf("unsetenv status: %d ,DEMO_ENV=%s\n", res, getenv("DEMO_ENV"));
}

jmp_buf jmp_buffer;

void demo_jmp()
{
    /* 提供跨函数的goto功能 */
    printf("enter demo_jmp()\n");
    /* 非局部跳转： 调用longjmp后，程序直接返回到 jmp_buffer中记录的栈帧处。 */
    //longjmp(jmp_buffer, -1);
}

void demo_limit()
{
    /* 查询和设置系统资源限制
     * getrlimit()
     * setrlimit()       */
}

/*---------------------------------------------------------------------------*/

struct command {
    char *name;
    int enable;

    void (*func)();
};

struct command cmds[] = {
        {"demo_bool",   1, demo_bool},
        {"demo_rand",   1, demo_rand},
        {"demo_malloc", 1, demo_malloc},
        {"demo_sysenv", 1, demo_sysenv},
        {"demo_jmp",    1, demo_jmp},
        {"demo_limit",  1, demo_limit},
        /* 命令模式 add more... */
        {NULL,          0, NULL},
};


int main()
{
    int i = 0;
    /* 记录该栈帧的一些信息（包括此处的返回地址），若返回值为0，则为首次调用。若非0，则是从别处跳过来的。 */
    int jmpno = setjmp(jmp_buffer);
    switch (jmpno) {
        case 0:
            // 首次调用，记录此时的栈信息，并保存在全局变量jmp_buffer中。
            break;
        case -1:
            printf("demo_jmp() error!");
            return 1;
        default:
            printf("other error! %d\n", jmpno);
            break;
    }
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
