//
// Created by longduping on 2019/11/1.
//

#ifndef C_LEARNING_ZMQ_UTIL_H
#define C_LEARNING_ZMQ_UTIL_H

#include <zmq.h>
#include <string.h>

#define MAX_RECV_LEN 10240

/**
 * 接收字符串
 * @param socket  ZMQ socket
 * @param max_len 最大接收最大长度，超过此长度的消息会被截断
 * @return 收到的消息
 */
static inline char *s_recv(void *socket, size_t max_len)
{
    if (max_len <= 0) {
        return NULL;
    }
    char buffer[max_len];
    max_len--;
    size_t size = zmq_recv(socket, buffer, max_len, 0);
    if (size == -1)
        return NULL;
    if (size > max_len)
        size = max_len;
    buffer[size] = '\0';
    return strdup(buffer);
}

/**
 * 发送字符串
 * @param socket ZMQ socket
 * @param string 字符串
 * @return 已发送的长度
 */
static inline int s_send(void *socket, char *msg)
{
    return zmq_send(socket, msg, strlen(msg), ZMQ_SNDMORE);
}

/**
 * 发送字符串 （通过消息结构体）
 * @param socket ZMQ socket
 * @param msg    欲发送的消息
 * @return 发送的长度
 */
static inline int s_send_msg(void *socket, char *msg)
{
    size_t len = strlen(msg), s_len;
    zmq_msg_t message;
    s_len = zmq_msg_init_size(&message, len);
    assert(s_len == 0);
    memcpy(zmq_msg_data(&message), msg, len);
    errno = 0;
    s_len = zmq_msg_send(&message, socket, ZMQ_DONTWAIT);
    if (s_len == -1 && errno == EAGAIN) {
        return EAGAIN;/* queue is full*/
    }
    zmq_msg_close(&message);
    return s_len;
}

/**
 * 接收字符串 （通过消息结构体）
 * @param socket ZMQ socket
 * @param s_len  消息的长度
 * @return 收到的消息
 */
static inline char *s_recv_msg(void *socket, int *s_len)
{
    size_t len;
    zmq_msg_t message;
    len = zmq_msg_init(&message);
    assert(len == 0);
    len = zmq_msg_recv(&message, socket, 0);
    assert(len != -1);
    char *string = malloc(len + 1);
    memcpy(string, zmq_msg_data(&message), len);
    zmq_msg_close(&message);
    string[len] = 0;
    if (s_len != NULL)
        *s_len = len;
    return (string);
}

#endif //C_LEARNING_ZMQ_UTIL_H
