//
// Created by longduping on 2019/10/23.
// 尾队列（双端队列）
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// queue定义所在头文件
#include <sys/queue.h>

struct data {
    char buf[1024];
    int buf_len;
} ;

struct entry {
    struct data d;
    TAILQ_ENTRY(entry) entries;
};

TAILQ_HEAD(tail_queue, entry) queue_head;

int main(int argc, char *argv[]) {
    printf("TAILQ Demo\n");


    // 初始化队列
    TAILQ_INIT(&queue_head);

    printf("queue_head: %p \n", &queue_head);
    printf("queue_head.tqh_first: %p \n", queue_head.tqh_first);
    printf("queue_head.tqh_last: %p \n", queue_head.tqh_last);

    // 尾插
    struct entry *item = (struct entry *) malloc(sizeof(struct entry *));
    item->d.buf_len = snprintf(item->d.buf, 1024, "hello,world.1");
    TAILQ_INSERT_TAIL(&queue_head, item, entries);

    // 头插
    struct entry *item2 = (struct entry *) malloc(sizeof(struct entry *));
    item2->d.buf_len = snprintf(item2->d.buf, 1024, "hello,world.2");
    TAILQ_INSERT_HEAD(&queue_head, item2, entries);

    // 返回第一个元素
    struct entry *e;
    e = TAILQ_FIRST(&queue_head);
    printf("%s\n", e->d.buf);

    // 删除指定元素
    TAILQ_REMOVE(&queue_head, e, entries);
    free(e);

    e = TAILQ_FIRST(&queue_head);
    printf("%s\n", e->d.buf);
    TAILQ_REMOVE(&queue_head, e, entries);
    free(e);


    return 0;
}
