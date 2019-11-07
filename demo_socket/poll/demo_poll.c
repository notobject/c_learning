/*
 * Created by longduping on 2019/11/6.
 *
 * poll 与 select 极其类似。区别在于：
 *
 * 1. poll 内部采用了链表结构，没有最大限制
 * 2. poll 针对流设备，能设置更详细监视条件
 *
 * @poll函数定义
 * extern int poll (struct pollfd *__fds, nfds_t __nfds, int __timeout);
 *
 * @param fds  pollfd结构数组，指定监视的文件描述符及其事件
 * @param nfds 数组长度
 * @param timeout  超时时间
 *              = -1, 永不超时（阻塞）
 *              = 0 ，立即返回（非阻塞）
 *              > 0 , 无事件则等待指定时间（部分阻塞）
 *              一般如果网络主循环是单独的线程的话，可以用-1来等，这样可以保证一些效率，
 *              如果是和主逻辑在同一个线程的话，则可以用0来保证主循环 的效率。
 * @return -1 发生异常，通过errno设置，如果errno为0，则为poll错误。否则为系统异常。
 *         >=0 产生事件的文件描述符数量
 *
 * @pollfd结构
 * fd 文件描述符
 * events 欲监视的事件（调用者填充，多个事件使用 ‘或’ 连接）
 * revents 已发生的事件（poll函数填充，多个事件使用 ‘或’ 连接）
 *
    event常量
     --------------------------------
    POLLIN	    普通或优先级带数据可读
    POLLRDNORM	普通数据可读
    POLLRDBAND	优先级带数据可读
    POLLPRI	    高优先级数据可读
    POLLOUT	    普通数据可写
    POLLWRNORM	普通数据可写
    POLLWRBAND	优先级带数据可写
     ---------------------------------以下3个事件由poll函数在事件发生时自动填充，无需显示指定
    POLLERR	    发生错误
    POLLHUP	    发生挂起
    POLLNVAL	描述字不是一个打开的文件
 */
#include <stdio.h>
#include <sys/poll.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int fd_size = 1;
    struct pollfd pfds[fd_size];
    pfds[0].fd = STDIN_FILENO;
    pfds[0].events = POLLIN;
    int len, i;
    char buffer[1024];
    while (1) {
        errno = 0;
        int ret = poll(pfds, 1, 0);

        switch (ret) {
            case -1:
                if (errno == EINTR) {
                    continue;
                }
                printf("[ERROR] %s\n", strerror(errno));
                break;
            case 0:
                // 无事件产生
                printf("[INFO] no events...\n");
                continue;
            default:
                // 需要进行一次轮询，判断究竟是哪些文件描述符产生了事件
                for (i = 0; i < fd_size; i++) {
                    len = 0;
                    if (pfds[i].revents & POLLIN) {
                        len = read(pfds[i].fd, buffer, 1024);
                    }
                    if (len > 0) {
                        printf("[%d][INFO] %s\n", pfds[i].fd, buffer);
                    }
                }
                break;
        }
    }

    return 0;
}
