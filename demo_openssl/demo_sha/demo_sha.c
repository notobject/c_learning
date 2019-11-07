//
// Created by longduping on 2019/11/5.
//

#include <openssl/sha.h>
#include <string.h>
#include <stdio.h>

int main()
{
    const unsigned char *src = "hello";
    unsigned char md[33] = {0};
    SHA256(src, strlen(src), md);
    char tmp[3] = {0};
    int i;
    char buf[65];
    for (i = 0; i < 32; i++) {
        sprintf(tmp, "%02X", md[i]);
        strcat(buf, tmp);
    }
    buf[65] = 0;
    printf("%s\n", buf);
    return 0;
}