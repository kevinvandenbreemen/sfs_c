/*
SFS In C Project:
Utility functions for working with and testing data in the SFS library
This file does not need to be exposed to code outside of the library
*/

#ifndef IN_SFS_C_UTIL
#define IN_SFS_C_UTIL

#include <stdio.h>
#include "../dependencies/log.c/src/log.h"

void sfs_bytes_debug(void *data, int length, int vertical) {
    char *format = "[%d] ";
    if(vertical) {
        format = "[%d] \n";
    }

    unsigned char *tmp = (unsigned char*)data;

    int i;
    for(i=0; i<length; i++) {
        printf(format, tmp[i]);
    }
    printf("\n");
}

#endif