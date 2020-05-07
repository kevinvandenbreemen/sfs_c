//  Lib implementation

#include "sfs_c.h"
#include "sfs_control.h"

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
    FILE *f = fopen(location, "wb");
    
    //  Write the prefix bytes
    char prefixBytes[6] = SFS_SIGNATURE;
    fwrite(prefixBytes, sizeof(char), 6, f);
    fclose(f);
    
    ChunkedFile *ret = malloc(sizeof(ChunkedFile*));
    ret->location = location;
    return ret;

}

/**
 * Read in the given number of bytes from the given position in the ChunkedFile
 */
static char* readBytesInternal(ChunkedFile *cf, long cursor, long numBytes) {
    FILE *f = fopen(cf->location, "r");
    if(f == NULL) {
        fprintf(stderr, "Failed to open %s for writing\n", cf->location);
        return NULL;
    }

    char *messageBytes = malloc(numBytes * sizeof(char));
    fseek(f, cursor, SEEK_SET);
    fread(messageBytes, sizeof(char), (numBytes), f);
    fclose(f);

    return messageBytes;
}

/**
 * Writes the given bytes to the given file at the given position.
 * Returns 0 if successful.
 */
static int writeBytesInternal(ChunkedFile *cf, long cursor, char *bytes, long numBytes) {

    FILE *f = fopen(cf->location, "r+b");
    if(f == NULL) {
        fprintf(stderr, "Failed to open %s for writing\n", cf->location);
        return 1;
    }

    if(fseek(f, cursor, SEEK_SET) != 0){
        fprintf(stderr, "Could not seek to position %ld in file %s\n", cursor, cf->location);
        return 1;
    }

    fwrite(bytes, sizeof(char), numBytes, f);
    fclose(f);

    return 0;
}

void sfs_setMessage(ChunkedFile *cf, char *message, int length) {

    int messageBytesLen = PREFIX_BYTE_LEN-SFS_SIGNATURE_LEN;
    char msgBytes[messageBytesLen];
    memset(msgBytes, 0, messageBytesLen);
    memcpy(msgBytes, message, length);

    msgBytes[length] = (char)SFS_CTRL_END_OF_HEADER;

    writeBytesInternal(cf, SFS_SIGNATURE_LEN, msgBytes, messageBytesLen);
}

void sfs_getMessage(ChunkedFile *cf) {

    int length = PREFIX_BYTE_LEN - SFS_SIGNATURE_LEN;
    char *messageBytes = readBytesInternal(cf, SFS_SIGNATURE_LEN, length);
    if(messageBytes == NULL) {
        return;
    }

    int indexOf = -1;
    int i = 0;
    for(i=0; i<length; i++) {
        if(messageBytes[i] == (char)SFS_CTRL_END_OF_HEADER) {
            indexOf = i;
            break;
        }
    }
    
    char *messagePayload = malloc(indexOf * sizeof(char));

    memcpy(messagePayload, messageBytes, indexOf);   
    free(messageBytes);

    cf->message = messagePayload;
    cf->messageLength = indexOf;

}