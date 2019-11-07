//
// Created by longduping on 2019/11/1.
//

#include "datetime.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    char *fs = NULL;

    fs = dt_date(NULL);
    printf("date: %s\n", fs);
    free(fs);

    fs = dt_time(NULL);
    printf("time: %s\n", fs);
    free(fs);

    fs = dt_datetime(NULL);
    printf("datetime: %s\n", fs);
    free(fs);


    printf("s : %ld\n", dt_s2seconds("2019-11-01 12:00:01", DATE_TIME_FORMAT_STR));
    printf("ms: %ld\n", dt_s2mseconds("2019-11-01 12:00:01", DATE_TIME_FORMAT_STR));
    printf("s : %ld\n", dt_seconds());
    printf("ms: %ld\n", dt_mseconds());
    return 0;
}
