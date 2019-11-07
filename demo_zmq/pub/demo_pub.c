//
// Created by longduping on 2019/11/1.
//

#include <zmq.h>
#include <stdlib.h>
#include <time.h>
#include "../common/zmq-util.h"


int main(int argc, char *argv[])
{
    //  准备上下文和PUB套接字
    void *context = zmq_init(1);
    void *publisher = zmq_socket(context, ZMQ_PUB);
    zmq_bind(publisher, "tcp://*:5556");
    zmq_bind(publisher, "ipc://weather.ipc");

    //  初始化随机数生成器
    srandom((unsigned) time(NULL));

    while (1) {
        //  生成数据
        int zipcode, temperature, relhumidity;
        zipcode = 10001;

        temperature = random() % (215) - 80;
        relhumidity = random() % (50) + 10;

        //  向所有订阅者发送消息
        char update[20];
        sprintf(update, "%05d %d %d", zipcode, temperature, relhumidity);
        printf("%s\n", update);
        s_send(publisher, update);
    }
    zmq_close(publisher);
    zmq_term(context);
    return 0;
    return 0;
}