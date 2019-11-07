//
// Created by longduping on 2019/10/17.
//

#include "demo_base64.h"

void demo_base64() {
    unsigned char *src = "asaksadjaksjdkjk=--49234735126379812@#$%^&*()5";
    unsigned char dst[100];
    int dlen = 65;
    printf("src=%s, slen=%d \n", src, strlen(src));
    int res = base64_encode(dst, &dlen, src, strlen(src));
    switch (res) {
        case XYSSL_ERR_BASE64_BUFFER_TOO_SMALL:
            printf("buffer too small. \n");
            break;
        case XYSSL_ERR_BASE64_INVALID_CHARACTER:
            printf("invalid character. \n");
            break;
        default:
            printf("dst=%s , dlen=%d\n", dst, dlen);
            break;
    }

    unsigned char decode_dst[65];
    int decode_dlen = 65;
    res = base64_decode(decode_dst, &decode_dlen, dst, strlen(dst));
    printf("decode_dst=%s , decode_dlen=%d\n", decode_dst, decode_dlen);
}