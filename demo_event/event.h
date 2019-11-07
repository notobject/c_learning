//
// Created by longduping on 2019/10/17.
//

#ifndef C_LEARNING_EVENT_H
#define C_LEARNING_EVENT_H


enum {
    E_INIT_ERROR = -1,
};

struct thread {
    pthread_t tid;
    char name[64];
};

#endif //C_LEARNING_EVENT_H
