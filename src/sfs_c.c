//  Lib implementation

#include "sfs_c.h"
#include <stdio.h>

int sfs_checkIsSFS(char *filePath) {

    FILE *filePointer;
    if((filePointer = fopen(filePath, "rb")) == NULL) {
        fprintf(stderr, "Could not open %s\n", filePath);
        return -1;
    }

    return 0;
}