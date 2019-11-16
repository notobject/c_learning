//
// Created by longduping on 2019/10/30.
//
#include <iniparser/iniparser.h>

int main(int argc, char *argv[])
{

    dictionary *dict = NULL;
    dict = iniparser_load("/tmp/tmp.sxPBKY2RCa/demo_conf/conf/app.conf");

    int i, nsec = iniparser_getnsec(dict);
    for(i = 0; i < nsec; i++){
        char *sec = iniparser_getsecname(dict, i);
        printf("[%s]\n", sec);
        int nkeys = iniparser_getsecnkeys(dict, sec);
        char **keys = iniparser_getseckeys(dict, sec, NULL);
        while (nkeys-- > 0){
            char *value = iniparser_getstring(dict, keys[nkeys], "NULL");
            printf("\tkey=%s, value=%s\n", keys[nkeys], value);
        }
    }

    printf("key1=%s\n", iniparser_getstring(dict, "section1:key1", "null"));
    printf("key2=%s\n", iniparser_getstring(dict, "section1:key2", "null"));
    printf("enable=%d\n", iniparser_getboolean(dict, "section2:enable", 0));
    printf("num=%d\n", iniparser_getint(dict, "section2:num", -1));
    printf("desc=%s\n", iniparser_getstring(dict, "global:desc", "unknown"));

    iniparser_freedict(dict);
    return 0;
}
