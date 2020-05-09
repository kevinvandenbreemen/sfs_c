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

/**
 * Default chunk size for SFS
 */
#define CHUNK_SIZE 160 * (3 * 1024)

//  INTERNAL/static FUNCTIONS NOT TO BE CONSUMED BY OTHER MODULES
//  =~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=

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


char* longToBytes(long long value) {
    char* longBytes = malloc(sizeof(char) * 8);
    
    int shiftFactor;
    int i=0;
    for(i=0; i<8; i++) {

        shiftFactor = 56 - (i * 8);

        if(i == 0) {
            longBytes[i] = (unsigned char)((value >> shiftFactor) & 0xFF);
        } else {
            longBytes[i] = (unsigned char)(value >> shiftFactor & 0xFF);
        }
    }

    return longBytes;
}

long long bytesToLong(char *bytes) {
    int i=0;
    //  From bytes
    long long sum = 0;
    
    int returnShiftFac;
    for(i=0; i<8; i++) {

        returnShiftFac = 56 - (i * 8);

        long rawByteData = (long long)((unsigned char)(bytes[i])) << returnShiftFac;
        sum |= rawByteData;
    }

    return sum;
}

char *encodeChunk(char *data, long long length) {

    int totalSize = 11;                      // At least has control bytes(Start of med, length ind, start of cont) and long for length
    char *lengthBytes = longToBytes(length); // which is 8 bytes long (long long data type storage requirement)

    totalSize += length;                    //  Total payload length

    char *chunkBytes = malloc(totalSize*sizeof(char));
    chunkBytes[0] = (char)START_OF_MEDIUM;
    chunkBytes[1] = (char)LENGTH_IND;

    int j = 2;                              //  Now start writing the data into the chunkBytes starting after the
                                            //  the two control bytes

    int i = 0;
    for(i=0; i<8; i++) {                    //  Write the 8 bytes of the stored long
        chunkBytes[j] = lengthBytes[i];
        j++;
    }

    chunkBytes[j] = (char)START_OF_CONTENT; //  Flag content start
    j++;
    memcpy(chunkBytes+j, data, length);     //  Write the actual data starting at chunkBytes pointer offset by len written so far

    return chunkBytes;
}

char *readChunk(char *data) {

    //  Step 1:  Determine the length of this chunk using control bytes
    if (data[0] != (char)START_OF_MEDIUM || data[1] != (char)LENGTH_IND || data[10] != START_OF_CONTENT) {
        fprintf(stderr, "Data set is not a recognized chunk (%d, %d, %d) \n", data[0], data[1], data[10]);
        exit(1);
    }

    long long length = bytesToLong(data + 2);
    char *readInData = malloc((int)length * sizeof(char));  //  Allocate length bytes to a new byte array for return

    //  Read in the content proper
    memcpy(readInData, data + 11, (int)length);             //  Read in length bytes starting at position 11 in the chunk

    return readInData;
}


//  =~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=

//  PUBLIC API IMPLEMENTATION
//  =~=~=~=~=~=~=~=~=~=~=~=~=

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
    
    ChunkedFile *ret = malloc(sizeof(ChunkedFile));
    ret->location = location;
    ret->unitSize = (int)CHUNK_SIZE;
    return ret;

}

ChunkedFile *sfs_openChunkedFile(char *location) {

    if(sfs_checkIsSFS(location) != 1){
        fprintf(stderr, "File at '%s' is not a chunked file.  Cannot continue and will exit to prevent data corruption\n", location);
        exit(1);
    }

    //  Load SFS
    ChunkedFile *ret = malloc(sizeof(ChunkedFile));
    ret->unitSize = (int)CHUNK_SIZE;
    ret->location = location;
    return ret;
    
}

char *sfs_readChunk(ChunkedFile *chunkedFile, long long atIndex) {
    //  Determine offset
    long long index = 0;
    index += (long long) PREFIX_BYTE_LEN;
    index += (atIndex * chunkedFile->unitSize);

    char *chunkData = readBytesInternal(chunkedFile, index, (long)chunkedFile->unitSize);
    return readChunk(chunkData);
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