//
// Created by longduping on 2019/10/28.
// 双向链表
//
#include <stdlib.h>
#include <stdio.h>
#include <sys/queue.h>

// 定义数据域
struct data {
    int id;
    char name[24];
};

// 定义链域
struct list_entry {
    struct data d;
    LIST_ENTRY(list_entry) entries;
};

// 定义头节点
LIST_HEAD(mylist, list_entry) list_head = LIST_HEAD_INITIALIZER(list_entry);

int main()
{
    // 初始化
    LIST_INIT(&list_head);

    // 判空
    printf("%s\n", LIST_EMPTY(&list_head) ? "True" : "False");

    // 头插
    struct list_entry *item = (struct list_entry *) malloc(sizeof(struct list_entry));
    item->d.id = 1;
    sprintf(item->d.name, "item-%d", item->d.id);
    LIST_INSERT_HEAD(&list_head, item, entries);

    printf("%s\n", LIST_EMPTY(&list_head) ? "True" : "False");

    // 获取第一个元素
    struct list_entry *item2 = LIST_FIRST(&list_head);
    printf("id=%d, name=%s\n", item2->d.id, item2->d.name);

    // 插入元素
    struct list_entry *item3 = (struct list_entry *) malloc(sizeof(struct list_entry));
    item3->d.id = 3;
    sprintf(item3->d.name, "item-%d", item3->d.id);
    LIST_INSERT_AFTER(item, item3, entries);

    // 遍历
    printf("\n");
    struct list_entry *e;
    LIST_FOREACH(e, &list_head, entries) {
        printf("id=%d, name=%s\n", e->d.id, e->d.name);
    }

    // ....略
    return 0;
}