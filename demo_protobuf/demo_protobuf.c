//
// Created by longduping on 2019/11/1.
//

/* 1. 编写proto文件
 *
 * 2. 生成 .c/.h文件
 *    protoc-c --c_out=. demo.proto
 *
 */

#include <stdio.h>
#include "demo.pb-c.h"

int main(int argc, char *argv[])
{
    // 初始化消息结构
    struct _Demo__DEMO demo = DEMO__DEMO__INIT;
    // 赋值
    demo.username = "1";
    demo.password = "2";
    demo.role = 1001;
    demo.timestamp = 1234567;
    // 获取打包后的长度
    int size = demo__demo__get_packed_size(&demo);
    uint8_t out[size];
    // 打包
    int len = demo__demo__pack(&demo, out);
    // 输出
    printf("%d, %d, %s\n", size, len, out);

    return 0;
}