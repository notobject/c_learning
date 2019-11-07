//
// Created by longduping on 2019/10/17.
//
#include "demo_nids.h"

void tcp_callback(struct tcp_stream *ssn, void **parm) {
    printf("nids_state: %d", ssn->nids_state);

    ssn->client.collect++;
    ssn->server.collect++;
}

int main() {
    // 初始化nids
    nids_params.dev_addon = -1;

    int ret = nids_init();
    printf("nids init return %d", ret);
    if (ret != 0) {
        return 0;
    }
    // 注册tcp callback函数
    nids_register_tcp(tcp_callback);

    // loop
    nids_run();

    nids_exit();
    return 0;
}