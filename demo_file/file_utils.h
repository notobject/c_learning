//
// Created by longduping on 2019/11/1.
//

#ifndef C_LEARNING_FILE_UTILS_H
#define C_LEARNING_FILE_UTILS_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

ssize_t file_get_size_by_fd(int _fd)
{
    struct stat fs;
    fstat(_fd, &fs);
    return fs.st_size;
}

ssize_t file_get_size_by_name(const char *filename)
{
    struct stat fs;
    stat(filename, &fs);
    return fs.st_size;
}

char *file_read_str(const char *filename, ssize_t *length)
{
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("[ERROR] file_read_str(): %s\n", strerror(errno));
    }
    char *line = NULL;
    size_t line_len = -1;
    char *content = NULL;
    ssize_t read, total = 0;
    while ((read = getline(&line, &line_len, fp)) != EOF) {
        content = (char *) realloc(content, total + read);
        strcat(content, line);
        total += read;
    }
    *length = total;
    fclose(fp);
    return content;
}

int file_write_str(const char *filename, const char *content, ssize_t length)
{
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("[ERROR] file_write_str(): %s\n", strerror(errno));
        return -1;
    }
    ssize_t w_len = fwrite(content, sizeof(char), length, fp);
    if (w_len == -1) {
        printf("[ERROR] file_write_str(): %s\n", strerror(errno));
        return -1;
    }
    fclose(fp);
    return 0;
}

#endif //C_LEARNING_FILE_UTILS_H
