#include "../src/sfs_c.h"
#include <stdlib.h>

#define TEST_FILE "testOutput/testFile"

int main(int argc, char const *argv[])
{
    char *filePath = TEST_FILE;
    ChunkedFile *cf = sfs_createChunkedFile(filePath);

    char message[8] = {'p','e','r','f','t','e','s','t'};
    sfs_setMessage(cf, message, 8);
    sfs_setMessage(cf, "Some other message", 18);

    //  Cleanup
    free(cf);

    //  Read in an existing indexed file
    cf = sfs_openChunkedFile("resource/indexedFile");
    char *data = sfs_readChunk(cf, 1);

    int i;
    printf("Chunk Data:\n");
    for(i=0; i<cf->unitSize; i++) {
        printf("%c", data[i]);
    }

    free(data);
    free(cf);

    return 0;
}
