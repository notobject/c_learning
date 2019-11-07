#include <stdio.h>
#include "demos/demo_nids.h"
#include "demos/demo_base64.h"
#include "demos/demo_mempool.h"
#include "demos/demo_pfring.h"
#include "demos/roundup_power_of_two.h"

#define TURE 1
#define FLASE 0

struct test_case {
    char *name;
    int8_t enable;
    void (*func)();
};

struct test_case cases[] = {
        {"base64", TURE, demo_base64},
        {"mempool", TURE, demo_mempool},
        {"nids", TURE, demo_nids},
        {"pfring", TURE, demo_pfring},
        /* add more... */
        {NULL, FLASE, NULL},
};

int main() {
    int i = 0;

    while (cases[i].func){
        if (!cases[i].enable) continue;
        printf("==========%s begin ============ \n", cases[i].name);
        cases[i].func();
        printf("\n");
        i++;
    }
    printf("\n\n==========all done.================ \n");
    return 0;
}
/*
int main3(int argc, char *argv[]) {
    struct dequeue *dq = dequeue_create();

    printf("empty()? %s \n", dequeue_empty(dq) == 1 ? "True" : "False");
    printf("size()? %d \n", dequeue_size(dq));

    struct event *e = (struct event *) malloc(sizeof(struct event *));
    e->event_id = 1;
    dequeue_push(dq, e);

    struct event *e2 = (struct event *) malloc(sizeof(struct event *));
    e2->event_id = 2;
    dequeue_push(dq, e2);

    struct event *e3 = (struct event *) malloc(sizeof(struct event *));
    e3->event_id = 3;
    dequeue_push(dq, e3);

    struct event *e4 = (struct event *) malloc(sizeof(struct event *));
    e4->event_id = 4;
    dequeue_push(dq, e4);

    struct event *e5 = (struct event *) malloc(sizeof(struct event *));
    e5->event_id = 5;
    dequeue_push(dq, e5);

    printf("empty()? %s \n", dequeue_empty(dq) == 1 ? "True" : "False");
    printf("size()? %d \n", dequeue_size(dq));

//    struct entry *p = dq->head;
//    while (p->next != dq->head) {
//        printf("%d \n", p->next->e->event_id);
//        p = p->next;
//    }


    int size = dequeue_size(dq), i = 0;
    for (i = 0; i < size; ++i) {
        struct event *e_tmp = dequeue_pop(dq);
        printf("%d \n", e_tmp->event_id);
    }
    printf("empty()? %s \n", dequeue_empty(dq) == 1 ? "True" : "False");
    printf("size()? %d \n", dequeue_size(dq));

    dequeue_free(dq);

    exit(0);
}
int main2(int argc, char *argv[]) {
    // C语言中的布尔类型, 任何非0值都会被转为1
    _Bool is_ok = 100;

    // C99提供的stdbool.h库，可以使用bool代替_Bool，用true,false分别表示1，0。从而让代码与C++兼容
    bool cpp_like_bool = true;
    printf("_Bool = %d \n", is_ok);
    printf("cpp_like_bool= %d \n", cpp_like_bool);

    // 随机数,stdlib.h
    srand((unsigned int) time(NULL));
    int n = 5;
    while (n-- > 0) {
        printf("%d \n", rand() % 100);
    }

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

    printf("Hello World");
    return 0;
}
 */