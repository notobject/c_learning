//
// Created by longduping on 2019/10/30.
//
/* 消息摘要算法
 * 1. MD  (Message Digest) 消息摘要
 * 2. SHA (Security Hash Algorithm) 安全散列算法
 * 3. MAC (Message Authentication Code) 消息认证码
 *
 * 主要作用： 验证数据完整性，用于数字签名。
 */
#include <stdio.h>
#include <string.h>
#include "md5.h"
#include "sha1.h"
#define MD5_STR_LEN_32 32
#define MD5_STR_LEN_16 16
#define SHA1_STR_LEN 32
#define SHA2_STR_LEN 60

int main(int argc, char *argv[])
{
    // MD5
    unsigned char md5[MD5_STR_LEN_32];
    char *data = "admin";
    get_md5(md5, data, strlen(data), MD5_UPPER);
    printf("%s's md5 is %s\n", data, md5);

    get_md5(md5, md5, strlen(md5), MD5_UPPER);
    printf("%s's md5 is %s\n", data, md5);


    // SHA-1
    char sha1[SHA1_STR_LEN];
    StrSHA1(data, strlen(data), sha1);
    printf("%s's sha1 is %s\n", data, sha1);
    // SHA-256
    return 0;
}