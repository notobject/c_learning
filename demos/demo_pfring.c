//
// Created by longduping on 2019/10/17.
//
#include "demo_pfring.h"
#include <stdio.h>
#include "pfring.h"
#include "pfring_zc.h"

void demo_pfring() {
    pfring *pf = pfring_open("eth0", MAX_CAPLEN, 0);

    printf("%s \n", pf->device_name);

    printf("done");
    pfring_close(pf);
}

