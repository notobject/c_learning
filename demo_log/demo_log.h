//
// Created by longduping on 2019/11/6.
//

#ifndef C_LEARNING_DEMO_LOG_H
#define C_LEARNING_DEMO_LOG_H

#include <stdio.h>


#define YEAR ((((__DATE__ [7] - '0') * 10 + (__DATE__ [8] - '0')) * 10 \
    + (__DATE__ [9] - '0')) * 10 + (__DATE__ [10] - '0'))

#define MONTH (__DATE__ [2] == 'n' ? (__DATE__ [1] == 'a' ? 1 : 6)  \
    : __DATE__ [2] == 'b' ? 2 \
    : __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? 3 : 4) \
    : __DATE__ [2] == 'y' ? 5 \
    : __DATE__ [2] == 'l' ? 7 \
    : __DATE__ [2] == 'g' ? 8 \
    : __DATE__ [2] == 'p' ? 9 \
    : __DATE__ [2] == 't' ? 10 \
    : __DATE__ [2] == 'v' ? 11 : 12)

#define DAY ((__DATE__ [4] == ' ' ? 0 : __DATE__ [4] - '0') * 10 \
    + (__DATE__ [5] - '0'))


#ifdef FLOG
#undef FLOG
#endif
#define FLOG(level, stream, fmt, args...) \
fprintf(stream,"%d-%d-%d %s %s():%d [%s] " fmt "\n", YEAR, MONTH, DAY, __TIME__, __FUNCTION__, __LINE__, level, ##args)

#ifdef DEBUG
#undef DEBUG
#endif
#define DEBUG(fmt, args...) FLOG("DEBUG", stdout, fmt, ##args)

#ifdef INFO
#undef INFO
#endif
#define INFO(fmt, args...) FLOG("INFO", stdout, fmt, ##args)

#ifdef WARN
#undef WARN
#endif
#define WARN(fmt, args...) FLOG("WARN", stderr, fmt, ##args)

#ifdef ERROR
#undef ERROR
#endif
#define ERROR(fmt, args...) FLOG("ERROR", stderr, fmt, ##args)

#endif //C_LEARNING_DEMO_LOG_H
