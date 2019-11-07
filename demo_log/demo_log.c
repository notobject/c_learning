//
// Created by longduping on 2019/11/6.
//

#include "demo_log.h"
#include <string.h>
#include <time.h>

int main()
{
    char *level = "INFO";
    char *msg = "sasadsadad";
    printf("%d-%d-%d %s %s():%d [%s] %s\n", YEAR, MONTH, DAY, __TIME__, __FUNCTION__, __LINE__, level, msg);

    ERROR("%s", msg);
    INFO("%s", msg);
    WARN("%s", msg);
    DEBUG("%s", msg);


    return 0;
}
