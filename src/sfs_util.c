/*
SFS In C Project:
Utility functions for working with and testing data in the SFS library
This file does not need to be exposed to code outside of the library
*/

#ifndef IN_SFS_C_UTIL
#define IN_SFS_C_UTIL

#include <stdio.h>
#include "../dependencies/log.c/src/log.h"

/**
 * Print out 
 */
void sfs_bytes_debug(char *message, void *data, int length, int vertical) {
    #ifndef SFS_DEBUG
    return;
    #endif

    char *format = "[%d] ";
    if(vertical) {
        format = "[%d] \n";
    }

    unsigned char *tmp = (unsigned char*)data;

    printf("%s\n", message);
    int i;
    int isZeroCount = 0;
    for(i=0; i<length; i++) {
        if(tmp[i] == '\0') {
            isZeroCount++;
        } else {
            if(isZeroCount >= 3){
                printf (" ... (%d) ... ", isZeroCount);
            }
            isZeroCount = 0;
        }

        if(isZeroCount < 3) {
            printf(format, tmp[i]);
        }
    }

    if(isZeroCount > 0) {
        printf(" ...(%d) ", isZeroCount);
    }

    printf("\n");
}

#endif