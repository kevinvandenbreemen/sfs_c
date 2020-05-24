#include "../src/sfs_chunkedfile.h"
#include "../src/sfs_crypt.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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
    for(i=0; i<14; i++) {
        printf("%c", data[i]);
    }

    free(data);
    free(cf);

    cf = sfs_createChunkedFile("testOutput/perftestWriteChunk");
    sfs_writeChunk(cf, 0, "writeCHUNK", 10);
    free(cf);

    //  Crypto Tests
    sfs_startup();

    char *string = "HelloWorld To Encrypt";
    void *encrypted = sfs_encrypt(string, "test", strlen(string));
    sfs_bytes_debug("Encrypted", encrypted, 32, 0);

    char *decrypted = sfs_decrypt(encrypted, "test", strlen(string));
    sfs_bytes_debug("Decrypted", decrypted, strlen(string), 0);

    free(encrypted);
    free(decrypted);

    return 0;
}
