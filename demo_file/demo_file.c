//
// Created by longduping on 2019/11/1.
//

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include "file_utils.h"
/* 不带缓冲的I/O
 *     open()
 *     close()
 *     lseek()
 *     ...
 */

/* 标准IO
 *
 */
int main()
{
    printf("标准输入文件描述符： %d\n", STDIN_FILENO); /* 标准输入流:stdin  */
    printf("标准输出文件描述符： %d\n", STDOUT_FILENO);/* 标准输出流:stdout */
    printf("标准错误文件描述符： %d\n", STDERR_FILENO);/* 标准错误流:stderr */
    printf("进程能够打开的最大文件数： %d\n", FOPEN_MAX); /* 只是标准定义，实际上对常用的系统而言，几乎是无限制的 */


    // 写文件
    char *str = "hello world";
    file_write_str("/tmp/abc.txt", str, strlen(str));

    // 读文件
    ssize_t length;
    char *content = file_read_str("/tmp/abc.txt", &length);
    printf("%lu: %s", length, content);
    free(content);


    return 0;
}