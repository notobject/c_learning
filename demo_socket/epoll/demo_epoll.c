/*
 * Created by longduping on 2019/11/6.
 *
 * 关于 epoll
 * 1. 无最大并发数限制
 * 2. 支持水平触发和边缘触发
 * 3. 采用事件回调机制，而非轮询，不会随着fd增加，性能线性下降。epoll只又"活跃"连接才会回调，跟总连接数无关。
 * 4. 内存拷贝，基于mmap,用户态与内核共享内存区域，减少内存拷贝开销。
 *
 * @函数定义
 *
 * int epoll_create (int __size)
 *
 * int epoll_wait (int __epfd, struct epoll_event *__events, int __maxevents, int __timeout)
 *
 * int epoll_ctl (int __epfd, int __op, int __fd, struct epoll_event *__event)
 */
#include <sys/epoll.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_EVENTS 1024
#define EPOLL_EVENT_SIZE 1

int main(int argc, char *argv[])
{
    int i, read_len;
    char buffer[1024];
    int epoll_fd = epoll_create(EPOLL_EVENT_SIZE);
    struct epoll_event *ev = (struct epoll_event *) malloc(EPOLL_EVENT_SIZE * sizeof(struct epoll_event));
    (ev + 0)->data.fd = STDIN_FILENO;
    (ev + 0)->events = EPOLLIN | EPOLLET;

    // 将事件加入到监听事件列表中
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, ev);

    while (1) {
        // 等待事件
        int nfds = epoll_wait(epoll_fd, ev, MAX_EVENTS, 0);
        // 无活跃事件
        if (nfds == 0) continue;
        // 发生异常
        if (nfds < 0) {
            printf("[ERROR] %s\n", strerror(errno));
            continue;
        }
        // 有活跃事件，遍历活跃事件，并处理
        for (i = 0; i < nfds; i++) {
            // 如果文件描述符为STDIN_FILENO 并且是 EPOLLIN 事件，则读取该流中的数据并输出
            if ((ev + i)->data.fd == STDIN_FILENO && (ev + i)->events & EPOLLIN) {
                read_len = read((ev + i)->data.fd, buffer, 1024);
                buffer[read_len] = 0;
                printf("[INFO] %s -> %s\n", "onStdInput()", buffer);
            }
        }
    }
    return 0;
}
