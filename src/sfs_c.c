//  Lib implementation

#include "sfs_c.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Standard signature of an SFS file.  This signature intended to make the file
 * look like an NES ROM when it isn't (hence the 'just kidding' afterward)
 */
#define SFS_SIGNATURE { 'N', 'E', 'S', 26, 'J', 'K' }
#define SFS_SIGNATURE_LEN 6

/**
 * Maximum number of bytes available in the prefix (including the SFS signature)
 */
#define PREFIX_BYTE_LEN 256

int sfs_checkIsSFS(char *filePath) {

    FILE *filePointer;
    if((filePointer = fopen(filePath, "rb")) == NULL) {
        fprintf(stderr, "Could not open %s\n", filePath);
        return -1;
    }

    char prefixBuffer[6];
    if (fread(prefixBuffer, 1, 6, filePointer) == 6) {
        char expectedBytes[6] = SFS_SIGNATURE;
        int i;
        for(i=0; i<6; i++) {
            if(expectedBytes[i] != prefixBuffer[i]) {
                return -1;
            }
        }

        return 1;
    }
    

    return 0;
}

ChunkedFile *sfs_createChunkedFile(char *location) {

    //  Create the file
    FILE *f = fopen(location, "w");
    
    //  Write the prefix bytes
    char prefixBytes[6] = SFS_SIGNATURE;
    fwrite(prefixBytes, sizeof(char), 6, f);
    
    ChunkedFile *ret = malloc(sizeof(ChunkedFile*));
    ret->location = location;
    return ret;

}

void sfs_setMessage(ChunkedFile *cf, char *message, int length) {

    char *location = *(cf->location);

    FILE *f = fopen(cf->location, "r+a");
    if(f == NULL) {
        fprintf(stderr, "Failed to open %s for writing\n", cf->location);
        return;
    }
    int messageBytesLen = PREFIX_BYTE_LEN-SFS_SIGNATURE_LEN;
    char msgBytes[messageBytesLen];
    memcpy(msgBytes, message, length);
    fseek(f, SFS_SIGNATURE_LEN, SEEK_SET);
    fwrite(msgBytes, sizeof(char), messageBytesLen, f);
}