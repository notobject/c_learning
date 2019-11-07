//
// Created by longduping on 2019/10/25.
//
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void daemonize()
{
    int pid, sid;
    pid = fork();

    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) {
        printf("pid: %d\n", pid);
        exit(EXIT_SUCCESS);
    }

    sid = setsid(); // 创建一个新的会话，使fork出的进程与终端脱离
    /*
     * 之前parent和child运行在同一个session里,parent是会话（session）的领头进程。
     * parent进程作为会话的领头进程，如果exit结束执行的话，那么子进程会成为孤儿进程，并被init收养。
     * 执行setsid()之后,child将重新获得一个新的会话(session)id。
     * 这时parent退出之后,将不会影响到child了。*/
    if (sid < 0) exit(EXIT_FAILURE);
    if (chdir("/") < 0) exit(EXIT_FAILURE);

    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    close(STDIN_FILENO);
}


int main()
{

    printf("I running at main process.\n");
    nice(-1);
    daemonize();
    // 后续代码在fork出的子进程中运行， 原parent进程已调用exit退出了。
    // 此时fork出的子进程已经成为新的会话的领头进程。
    while (1) {

    }
}

