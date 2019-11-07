/*
 * Created by longduping on 2019/11/5.
 *
 * 最原始的阻塞I/O模型
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <libnet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>


typedef size_t (*msg_producer)(char *, int);

typedef void (*msg_consumer)(int, void *);

void default_msg_consumer(int client_fd, void *msg)
{
    puts((char *) msg);
    send(client_fd, "OK\r\n", 4, 0);
}

size_t default_msg_producer(char *buffer, int len)
{
    return fgets(buffer, len, stdin) == NULL ? -1 : strlen(buffer);
}

void *recv_msg_run(void *sock_p)
{
    int sock = (int) *((long *) sock_p);
    char recv_buffer[512];
    int recv_len;
    while ((recv_len = recv(sock, recv_buffer, 512, 0)) > 0) {
        recv_buffer[recv_len] = 0;
        printf("%s", recv_buffer);
    }
    printf("Connection closed by foreign host.\n");
    exit(0);
}


void server(char *host, int port, msg_consumer consumer)
{
    struct sockaddr_in addr_in;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    addr_in.sin_family = AF_INET;
    addr_in.sin_addr.s_addr = inet_addr(host);
    addr_in.sin_port = htons(port);
    bzero(&addr_in.sin_zero, sizeof(addr_in.sin_zero));
    if (bind(sock, (struct sockaddr *) &addr_in, sizeof(struct sockaddr)) == -1) {
        printf("[ERROR] bind socket failed: %s\n", strerror(errno));
        exit(errno);
    }
    // n： TCP完成3次握手 的数据包队列长度
    if (listen(sock, 1024) == -1) {
        printf("[ERROR] listen failed: %s\n", strerror(errno));
        exit(errno);
    }
    printf("[%s:%d] listen...\n", inet_ntoa(addr_in.sin_addr), port);
    if (!consumer) {
        consumer = default_msg_consumer;
    }
    /* handle client connections */
    struct sockaddr_in addr_client;
    unsigned int sin_size = sizeof(struct sockaddr);
    int client_sock, client_port, addr_len;
    char *recv_buf = (char *) malloc(1024);
    ssize_t recv_len = -1;
    char *hostname;
    char s[10240];
    /*
     * P0: 阻塞I/O
     * accept之后，在同一个线程中进行处理。这是最原始的阻塞I/O模型。
     * 优点是编程简单
     * 缺点是不满足并发（该线程只能处理一个连接，阻塞在recv()函数）。
     *
     * P1：多线程处理模型
     * 这里每次accept之后，可以创建一个线程，将该连接丢给线程去处理。
     * 优点是满足大量连接并发
     * 但是系统的效率随着连接数上升而迅速下降（系统对线程调度的开销会越来越大）
     *
     * P2: 基于线程池的多线程处理模型
     * 改成每次accept之后，从线程池中取一个线程，将该连接丢给线程去处理。
     * 该模型节省了线程的创建，销毁及系统调度的开销。
     * 支持的连接数也是有限的，随着大量连接并发，线程池所有线程都处于活跃状态，依然会降低系统效率。
     *
     * P3：I/O多路复用模型
     * 详见 demo_select / demo_poll / demo_epoll
     */
    while ((client_sock = accept(sock, (struct sockaddr *) &addr_client, &sin_size)) != -1) {

        hostname = inet_ntoa(addr_client.sin_addr);
        if (!hostname) {
            printf("[ERROR] convert remote addr failed: %s\n", strerror(errno));
            continue;
        }
        client_port = ntohs(addr_client.sin_port);
        addr_len = snprintf(s, 10240, "[%s:%d] ", hostname, client_port);
        printf("%sconnected.\n", s);
        while ((recv_len = recv(client_sock, recv_buf, 1024, 0)) != -1) {
            char *tmp = s + addr_len;
            recv_buf[recv_len] = 0;
            while (*recv_buf) {
                *tmp++ = *recv_buf++;
            }
            *tmp = '\0';
            consumer(client_sock, s);
        }
    }
    free(recv_buf);
}


void client(char *host, int port, msg_producer producer)
{
    struct sockaddr_in addr;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(host);
    addr.sin_port = htons(port);
    bzero(addr.sin_zero, 8);
    if (connect(sock, (struct sockaddr *) &addr, sizeof(struct sockaddr)) == -1) {
        printf("[ERROR] connect to %s:%d failed: %s\n", host, port, strerror(errno));
        exit(errno);
    }
    printf("connect to [%s:%d]\n", host, port);
    if (!producer) {
        producer = default_msg_producer;
    }
    /* send and recv */
    char send_buffer[512];
    int send_len;
    pthread_t pid;
    if (pthread_create(&pid, NULL, recv_msg_run, (void *) (long) &sock) == -1) {
        printf("[ERROR] error to create recv thread: %s\n", strerror(errno));
        exit(errno);
    }
    while ((send_len = producer(send_buffer, 512)) != -1) {
        if (strcmp(send_buffer, "q\n") == 0) break;
        if (send(sock, send_buffer, send_len, 0) < 0) {
            printf("[ERROR] send %s failed: %s\n", send_buffer, strerror(errno));
            exit(errno);
        }
    }
}

int main()
{
    client("39.156.69.79", 80, default_msg_producer);
    //server("0.0.0.0", 2048, default_msg_consumer);
    return 0;
}