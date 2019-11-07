//
// Created by longduping on 2019/11/1.
//

#ifndef C_LEARNING_DATETIME_H
#define C_LEARNING_DATETIME_H

#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define DATE_FORMAT_STR "%Y-%m-%d"
#define TIME_FORMAT_STR "%H:%M:%S"
#define DATE_TIME_FORMAT_STR "%Y-%m-%d %H:%M:%S"

#define DATETIME_STR_LEN 64

// 获取时间戳 或 当前（timestamp == NULL） 日期 指定格式化字符串
static inline char *datetime_format(time_t timestamp, const char *fmt)
{
    if ((void *) timestamp == NULL) {
        timestamp = time(NULL); /* 获取当前日历时间 */
    }
    char *buf = (char *) malloc(DATETIME_STR_LEN); /* 须调用后手动释放 */
    // localtime(&t) 将日历时间转为本地时间
    // gmtime(%t) 将日历时间转为协调统一时间
    if (strftime(buf, DATETIME_STR_LEN, fmt, localtime(&timestamp)) == 0) { /*将本地时间格式化输出 */
        return NULL;
    }
    return buf;
}

// 获取时间戳 或 当前（timestamp == NULL） 日期 默认格式化字符串
static inline char *dt_date(time_t timestamp)
{
    return datetime_format(timestamp, DATE_FORMAT_STR);
}

// 获取时间戳 或 当前（timestamp == NULL） 时间 默认格式化字符串
static inline char *dt_time(time_t timestamp)
{
    return datetime_format(timestamp, TIME_FORMAT_STR);
}

// 获取时间戳 或 当前（timestamp == NULL） 日期和时间 默认格式化字符串
static inline char *dt_datetime(time_t timestamp)
{
    return datetime_format(timestamp, DATE_TIME_FORMAT_STR);
}

// 字符串转时间戳（秒）
static inline time_t dt_s2seconds(const char *s, const char *fmt)
{
    struct tm _tm;
    strptime(s, fmt, &_tm);
    time_t t = mktime(&_tm);
    return t;
}

// 字符串转时间戳（微妙）
static inline suseconds_t dt_s2mseconds(const char *s, const char *fmt)
{
    struct tm _tm;
    strptime(s, fmt, &_tm);
    time_t t = mktime(&_tm);
    return t * 1000;
}

// 获取时间戳（秒）
static inline time_t dt_seconds()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec;
}

// 获取时间戳（微妙）
static inline __suseconds_t dt_mseconds()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}


#endif //C_LEARNING_DATETIME_H
