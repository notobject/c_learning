//
// Created by longduping on 2019/10/30.
//
#include <unistd.h>
#include <stdio.h>
int main(int argc, char *argv[])
{
    if (argc < 2){
        printf("'%s -h' for help\n", argv[0]);
    }
    int ch;
    while ((ch = getopt(argc, argv, ":a:i:rf::")) != -1){
        printf("current opt index=%d\n", optind);
        switch (ch){
            case 'i':
                printf("-i: %s\n", optarg);
                break;
            case 'a':
                printf("-a: %s\n", optarg);
                break;
            case 'r':
                printf("-r:\n");
                break;
            case 'f':
                printf("-f:\n");
                break;
            case '?':
                // 无效的选项
                printf("Unknown option: -%c \n", optopt);
                break;
            case ':':
                // 缺少选项参数（getopt()的第三个参数必须以‘:’开始，才会将缺少选项参数和无效选项区分开，否则都返回‘?’）
                printf("miss argument: -%c \n", optopt);
                break;
        }
    }
    return 0;
}
