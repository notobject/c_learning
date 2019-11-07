//
// Created by longduping on 2019/11/1.
//
#include <zmq.h>
#include <stdlib.h>
#include "../common/zmq-util.h"

int main(int argc, char *argv[])
{
    void *context = zmq_init (1);

    //  创建连接至服务端的套接字
    printf ("正在收集气象信息...\n");
    void *subscriber = zmq_socket (context, ZMQ_SUB);
    zmq_connect (subscriber, "ipc://weather.ipc");

    //  设置订阅信息，默认为纽约，邮编10001
    char *filter = (argc > 1)? argv [1]: "10001 ";
    zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, filter, strlen (filter));

    //  处理100条更新信息
    int update_nbr;
    long total_temp = 0;
    for (update_nbr = 0; update_nbr < 100; update_nbr++) {
        char *string = s_recv (subscriber, 256);
        printf("%s\n", string);
        int zipcode, temperature, relhumidity;
        sscanf (string, "%d %d %d",
                &zipcode, &temperature, &relhumidity);
        total_temp += temperature;
        free (string);
    }
    printf ("地区邮编 '%s' 的平均温度为 %dF\n",
            filter, (int) (total_temp / update_nbr));

    zmq_close (subscriber);
    zmq_term (context);
    return 0;
}