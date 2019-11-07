//
// Created by longduping on 2019/11/5.
//

#include <openssl/evp.h>
#include <openssl/aes.h>
#define BUFFER_LENGTH 1024

typedef enum {
    MODE_DECRYPT = 0,
    MODE_ENCRYPT
} MODE;

void evp_aes_crypt(const unsigned char *in,
                     const int in_len,
                     unsigned char *out,
                     int *out_len,
                     const unsigned char *key,
                     const unsigned char *iv,
                     MODE mode)
{
    // 输入输出缓冲
    //unsigned char in_buf[BUFFER_LENGTH];
    //unsigned char out_buf[BUFFER_LENGTH + EVP_MAX_BLOCK_LENGTH];
    // 创建上下文
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();

    // 初始化，设置算法和加解密模式
    EVP_CipherInit_ex(ctx, EVP_aes_128_cfb(), NULL, NULL, NULL, mode);
    OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == 16);
    OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == 16);
    // 初始化，设置密钥和初始向量
    EVP_CipherInit_ex(ctx, NULL, NULL, key, iv, mode);

    if (!EVP_CipherUpdate(ctx, out, out_len, in, in_len)) {
        /* Error */
        EVP_CIPHER_CTX_free(ctx);
        return;
    }
    if (!EVP_CipherFinal_ex(ctx, out, out_len)) {
        /* Error */
        EVP_CIPHER_CTX_free(ctx);
        return;
    }
    EVP_CIPHER_CTX_free(ctx);
}


void main()
{
//    EVP加解密框架 —— AES加解密
    unsigned char key[] = "0123456789abcdeF";
    unsigned char iv[] = "1234567887654321";
    int out_len, in_len;
    unsigned char out[512];
    unsigned char in[512];

    in_len = snprintf(in, 512, "%s", "admin");
    evp_aes_crypt(in, in_len, out, &out_len, key, iv, MODE_ENCRYPT);
    out[out_len] = 0;
    printf("%s -> %s\n", in, out);

    evp_aes_crypt(out, out_len, in, &in_len, key, iv, MODE_DECRYPT);
    in[in_len] = 0;
    printf("%s -> %s\n", out, in);


}
