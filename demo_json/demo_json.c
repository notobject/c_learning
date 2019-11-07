//
// Created by longduping on 2019/10/30.
//

#include <stdio.h>
#include <time.h>
#include "cJSON.h"


int main(int argc, char *argv[])
{
    // 创建JSON对象
    cJSON *root = cJSON_CreateObject();

    // string 对象
    cJSON_AddItemToObject(root, "string", cJSON_CreateString("i am String"));
    // bool 对象
    cJSON_AddItemToObject(root, "bool", cJSON_CreateBool(0));
    // 数值 对象
    cJSON_AddItemToObject(root, "number", cJSON_CreateNumber(100));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateNumber(time(NULL)));
    // null 对象
    cJSON_AddItemToObject(root, "null", cJSON_CreateNull());

    // 子节点
    cJSON *sub = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "sub", sub);

    // 对象数组
    cJSON *arr = cJSON_CreateArray();
    cJSON_AddItemToArray(arr, cJSON_CreateObject());
    cJSON_AddItemToObject(root, "arr", arr);

    // <字符串类型>数组
    const char *strs[] = {"str1", "str2"};
    cJSON *strArr = cJSON_CreateStringArray(strs, 2);
    cJSON_AddItemToObject(sub, "str_arr", strArr);

    // <数值类型>数组
    const float numbers[] = {3.14f, 3.15f, 3.16f};
    cJSON *fltArr = cJSON_CreateFloatArray(numbers, 3);
    cJSON_AddItemToObject(sub, "float_arr", fltArr);

    // 输出字符串
    char *json = cJSON_Print(root);
    // 打印可读格式
    printf("%s\n", json);

    // 打印紧凑格式
    cJSON_Minify(json);
    printf("%s\n", json);

    // 释放
    cJSON_Delete(root);
    return 0;
}

