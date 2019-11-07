//
// Created by longduping on 2019/10/24.
//
#include <stdio.h>
#include <magic.h>
#include <stdlib.h>
#include <string.h>


#define FILE_NAME_LEN 255
#define MIME_TYPE_LEN 64

struct magic_desc {
    char *file_name;
    char *mime_type;
    int head_len;
    int tail_len;
};

struct magic_desc m_desc[] = {
        {"/home/longduping/data/excel_97-2003.xls", "application/vnd.ms-excel", 512, 512},
        {"/home/longduping/data/excel_2016.xlsx",   "application/vnd.ms-excel", 512, 512},
        {NULL, NULL,                                                            0,   0},
};

void display_k(char *content, int k)
{
    int i;
    printf("\t00000000h: ");
    for (i = 0; i < k; i++)
        printf("%02X ", content[i]);
    printf("\n");
}


int main()
{

    const char *magic_file = "/opt/work/configs/magic.mgc";
    // load magic
    magic_t cookie = magic_open(MAGIC_NO_CHECK_ENCODING | MAGIC_MIME_TYPE);
    int ret = magic_load(cookie, magic_file);
    if (ret == -1) {
        magic_close(cookie);
        printf("load magic failed.\n");
    }

    // read from file
    char *content = NULL, *tmp_content = NULL;
    unsigned long content_len = 0;
    char buf[512];
    int len = 0, i;
    for (i = 0; m_desc[i].file_name != NULL; i++, content_len = 0) {

        FILE *fp = fopen(m_desc[i].file_name, "r");
        if (!fp) {
            printf("open file failed.\n");
            return -1;
        }

        while ((len = (int) fread(buf, sizeof(char), 512, fp)) > 0) {
            if (content_len > 0) {
                tmp_content = (char *) malloc(sizeof(char) * content_len);
                memcpy(tmp_content, content, content_len);
            }
            free(content);
            content = (char *) malloc((content_len + len) * sizeof(char));
            memcpy(content, tmp_content, content_len);
            memcpy(content + content_len, buf, len);

            content_len += len;
        }
        if (content && content_len > 0) {
            content[content_len] = '\0';
        }
        printf("%s\n", m_desc[i].file_name);
        display_k(content, 8);
        const char *all_mime_type, *sub_mime_type;
        all_mime_type = magic_buffer(cookie, content, content_len);
        printf("\tall: len=%lu, [0,%lu],\t mime_type=%s \n", content_len, content_len, all_mime_type);
        int sub_content_len = m_desc[i].head_len + m_desc[i].tail_len;
        char sub_content[sub_content_len];
        memcpy(sub_content, content, m_desc[i].head_len);
        memcpy(sub_content + m_desc[i].head_len, content + (content_len - m_desc[i].tail_len), m_desc[i].tail_len);
        sub_mime_type = magic_buffer(cookie, sub_content, sub_content_len);
        printf("\tsub: len=%d, [%d,%d],\t mime_type=%s\n", sub_content_len, m_desc[i].head_len, m_desc[i].tail_len,
               sub_mime_type);
        if (content != NULL) {
            free(content);
            content = NULL;
        }
        if (tmp_content != NULL) {
            free(tmp_content);
            tmp_content = NULL;
        }
        fclose(fp);
    }

    int num = 0;

    int read_count = 0;
    int socket_index;
    int send_success;
    int socket_total = 2;
    u_int32_t send_flag = 0;
    while (1) {
        num++;
        if (num > 3) {
            break;
        }
retry:
        read_count++;
        socket_index = read_count % socket_total;
        send_success = socket_index == 4 ? 0 : -1;
        printf("read_count=%d, socket_index=%d, send_success=%d\n", read_count, socket_index, send_success);
        if (send_success < 0) {
            send_flag |= (1 << socket_index);
            printf("\tsend_flag=%d, ((1 << socket_total) - 1)=%d\n", send_flag, ((1 << socket_total) - 1));
            if (send_flag == ((1 << socket_total) - 1)) {
                printf("\t\tcontiue;\n");
                send_flag = 0;
                continue;
            }
            goto retry;
        }
        printf("add 1!!\n");
    }

    magic_close(cookie);
    return 0;
}