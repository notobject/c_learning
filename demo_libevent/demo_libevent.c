/*
 * @Author: Long Duping 
 * @Date: 2019-12-02 11:16:06 
 * @Last Modified by: Long Duping
 * @Last Modified time: 2019-12-02 18:02:25
 */

#include <stdlib.h>
#include <stdio.h>
#include <event2/event.h>
#include <netinet/in.h>
#include <libnet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>

#define BUFFER_LEN 1024
void on_read(evutil_socket_t fd, short events, void *arg)
{
    
    char buffer[BUFFER_LEN];
    ssize_t len = 0;
    /* recv (only recv 1024 bytes) */
    len = recv(fd, buffer, BUFFER_LEN - 1, 0);
    if(len <= 0)
        return ;
    buffer[len] = '\0';

    fprintf(stdout, "%s", buffer);

    /* reply */
    char msg[64];
    int msg_len = snprintf(msg, 64, "\t recv %s", buffer);
    write(fd, msg, msg_len);
}

void on_accept(evutil_socket_t fd, short events, void *arg)
{
    int client_fd;
    struct event_base *ev_base = (struct event_base *)arg;

    /* do accept ... */
    struct sockaddr_in addr_in;
    socklen_t socklen = sizeof(addr_in);
    if((client_fd = accept(fd, (struct sockaddr *)&addr_in, &socklen)) < 0){
        perror("on accept");
        return ;
    }
    evutil_make_socket_nonblocking(fd);
    
    /* TODO: Using buffer event */
    
    struct event *ev = event_new(ev_base, client_fd, EV_READ | EV_PERSIST, on_read, arg);
    event_add(ev, NULL);

    return ;
}


int main(int argc, char const *argv[])
{
    struct sockaddr_in saddr_in;

    struct hostent *h = gethostbyname("localhost");
    
    saddr_in.sin_family = AF_INET;
    saddr_in.sin_addr.s_addr = inet_addr("0.0.0.0");
    saddr_in.sin_port = htons(8080);


    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(bind(sock_fd, (struct sockaddr *)&saddr_in, sizeof(struct sockaddr)) != 0){
        perror("on bind");
        return EXIT_FAILURE;
    }

    evutil_make_listen_socket_reuseable(sock_fd);

    if(listen(sock_fd, 10) != 0){
        perror("on listen");
        return EXIT_FAILURE;
    }

    struct event_base *ev_base =  event_base_new();
    struct event *e = event_new(ev_base, sock_fd, EV_READ| EV_PERSIST, on_accept, (void *) ev_base);
    
    evutil_make_socket_nonblocking(sock_fd);

    event_add(e, NULL);

    printf("[INFO] event dispatch...\n");
    event_base_dispatch(ev_base);

    return EXIT_SUCCESS;
}
