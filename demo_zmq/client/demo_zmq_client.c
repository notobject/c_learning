//
// Created by longduping on 2019/11/1.
//
/* ZMQ/0MQ 的消息模式
 * 1. Request-Reply 请求-回复
 * 2. Publish-Subscribe 发布-订阅
 * 3. Pipeline 管道（fan-out/fan-in）
 * 4. Exclusive pair 线程同步模式
 *
 */
#include <zmq.h>
#include <string.h>
#include <stdlib.h>
#include <zmq_utils.h>

int main(int argc, char *argv[])
{


    void *context = zmq_init (1);

    //  连接至服务端的套接字
    printf ("正在连接至hello world服务端...\n");
    void *requester = zmq_socket (context, ZMQ_REQ);
    if(zmq_connect (requester, "tcp://localhost:5555") != 0){
        printf("connect failed.");
        exit(-1);
    }

    int request_nbr;
    for (request_nbr = 0; request_nbr != 10; request_nbr++) {
        zmq_msg_t request;
        zmq_msg_init_size (&request, 5);
        memcpy (zmq_msg_data (&request), "Hello", 5);
        printf ("正在发送 Hello %d...\n", request_nbr);
        zmq_msg_send (requester, &request, 0);
        zmq_msg_close (&request);

        zmq_msg_t reply;
        zmq_msg_init (&reply);
        zmq_msg_recv (requester, &reply, 0);
        printf ("接收到 World %d\n", request_nbr);
        zmq_msg_close (&reply);
    }
    zmq_close (requester);
    zmq_term (context);
    printf("zme client closed!\n");
}