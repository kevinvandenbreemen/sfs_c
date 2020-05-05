#include "../src/sfs_c.h"
#include <stdlib.h>

#define TEST_FILE "testOutput/testFile"

int main(int argc, char const *argv[])
{
    char *filePath = TEST_FILE;
    ChunkedFile *cf = sfs_createChunkedFile(filePath);

    //  Cleanup
    free(cf);
    return 0;
}
