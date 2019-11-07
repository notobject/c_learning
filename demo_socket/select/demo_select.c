/*
 * Created by longduping on 2019/11/6.
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>

/*
    * @ select函数定义
    * int select (int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
    *
    * @param nfds: =所监视的文件描述符最大值 + 1
    * @param readfds: 需监视读变化（可读）的文件描述符集合
    *                 =NULL 不监控读变化
    * @param writefds: 需监视写变化（可写）的文件描述符集合
    *                  =NULL 不监控写变化
    * @param exceptfds: 需监视异常变化（错误）的文件描述符集合
    *                   =NULL 不监控错误变化
    * @param timeout: 超时时间
    *                 =NULL    select为完全阻塞模式（所监视文件描述符有变化才返回）
    *                 =零值    select为完全非阻塞模式（不管有没有变化，都不等待直接返回）
    *                 =非零值   select为部分非阻塞模式（若没有变化，则阻塞。阻塞设置的时间后还没有变化，则返回超时）
    *                 一般如果网络主循环是单独的线程的话，可以用NULL来等，这样可以保证一些效率，
 *                    如果是和主逻辑在同一个线程的话，则可以用0来保证主循环 的效率。
    * @return -1 错误， 0 超时， >0 所监视的文件描述符有变化（可读/可写/异常）
    *
    * @ 关于fd_set结构
    * fd_set *sets = (fd_set *) malloc(sizeof(fd_set));
    *
    * // select支持监控的文件描述符数量= 32 * int的位数
    * printf("FD_SETSIZE=%d\n", FD_SETSIZE);
    *
    * // 清空集合sets
    * FD_ZERO(sets);
    *
    * // 将文件描述符fd 加入集合sets
    * int fd = 0;
    * FD_SET(fd, sets);
    *
    * // 将文件描述符0 从集合sets删除
    * FD_CLR(0, sets);
    *
    * // 判断该文件描述符fd, 是否可读可写
    * printf("%s\n", FD_ISSET(fd, sets) ? "yes" : "no");
    */
int main(int argc, char *argv[])
{

    // int sock = socket(...);
    // bind(...)aaaaa
    // listen(...)
    // accept(client_fd, ....)
    int i, max_fd = 0, fd_size = 1;
    int fds[] = {0};

    fd_set readfds;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    int nfds, ret;
    char buffer[1024];
    while (1) {
        // 每次调用select都必须先清空fd_set， 然后重新设置
        FD_ZERO(&readfds);
        max_fd = -1;
        for (i = 0; i < fd_size; i++) {
            FD_SET(fds[i], &readfds);
            if (fds[i] > max_fd) {
                max_fd = fds[i];
            }
        }
        ret = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        switch (ret) {
            case -1:
                /* 常见错误：
                 * EBADF  文件描述词为无效的或该文件已关闭
                 * EINTR  此调用被信号所中断
                 * EINVAL 参数n 为负值.
                 * ENOMEM 核心内存不足
                 */
                printf("[ERROR] %d:%s\n", errno, strerror(errno));
                break;
            case 0:
                printf("[ERROR] %s\n", "time out!");
                break;
            default:
                // ok it's readable or writable
                // select为水平触发， 如果事件产生而没有处理，会一直产生。
                // 需要进行一次轮询，判断是哪些文件描述符产生了事件
                for (i = 0; i < fd_size; i++) {
                    if (FD_ISSET(fds[i], &readfds)) {
                        read(fds[i], buffer, 1024);
                        printf("[%d][INFO]ret=%d, do something...%s\n", fds[i], ret, buffer);
                    }
                }
                break;
        }
    }
    return 0;
}
