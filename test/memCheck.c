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
    return 0;
}
