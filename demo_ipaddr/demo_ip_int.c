/*
 * @Author: Long Duping 
 * @Date: 2019-12-13 17:08:17 
 * @Last Modified by: Long Duping
 * @Last Modified time: 2019-12-13 18:25:43
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* 点分ipv4地址转uint32 */
u_int32_t ip2int(const char *ip)
{
    char *p = ip, *offset = ip;
    int index = 3;
    u_int32_t ip_32 = 0;
    while (*p != '\0')
    {
        if (*p == '.')
        {
            *p = '\0';
            ip_32 |= atoi(offset) << (index-- * 8);
            offset = p + 1;
        }
        p++;
    }
    ip_32 |= atoi(offset);
    return ip_32;
}

/* uint32转点分ipv4地址 */
size_t int2ip(u_int32_t ip_32, char *buf, size_t len)
{
    return snprintf(buf, len, "%d.%d.%d.%d", (ip_32 >> 24) & 255U, ((ip_32 << 8) >> 24) & 255U, ((ip_32 << 16) >> 24) & 255U, ip_32 & 255U);
}


int main()
{
    char ip[] = "192.168.100.1";
    printf("ori ip: %s\n", ip);

    // ip to int
    u_int32_t ip_32 = ip2int(ip);
    printf("ip_32: %u \n", ip_32);

    // int to ip
    char buf[16]; //ipv4 
    int2ip(ip_32, buf, 16);
    printf("ip: %s\n", buf);

    return 0;
}