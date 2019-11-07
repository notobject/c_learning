//
// Created by longduping on 2019/11/1.
//
#include <zmq.h>
#include <stdio.h>
#include <zconf.h>
#include <string.h>
#include "zmq-util.h"

int main(int argc, char *argv[])
{
    void *context = zmq_init(1);

    //  与客户端通信的套接字
    void *responder = zmq_socket(context, ZMQ_REP);
    zmq_bind(responder, "tcp://*:5555");

    while (1) {
        //  等待客户端请求
        zmq_msg_t request;
        zmq_msg_init(&request);
        int r = zmq_msg_send(responder, &request, 0);
        if (r == -1)
            continue;
        printf("(%d)收到 Hello\n", r);
        zmq_msg_close(&request);

        //  做些“处理”
        sleep(1);

        //  返回应答
        zmq_msg_t reply;
        zmq_msg_init_size(&reply, 5);
        memcpy(zmq_msg_data(&reply), "World", 5);
        zmq_msg_send(responder, &reply, 0);
        zmq_msg_close(&reply);
    }
    //  程序不会运行到这里，以下只是演示我们应该如何结束
    zmq_close(responder);
    zmq_term(context);
}