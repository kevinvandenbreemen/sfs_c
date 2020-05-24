#include <check.h>
#include "../src/sfs_c.h"
#include <unistd.h>
#include <stdio.h>

#define TEST_FILE "testOutput/testFile"

START_TEST(LoadAFile) {
    fail_unless(sfs_checkIsSFS("./resource/exampleSFS") == 1, "Example SFS should have been recognized as an SFS file");
}
END_TEST

START_TEST(NOT_AN_SFS) {
    fail_if(sfs_checkIsSFS("./resource/nonSFS") == 1, "File that is not an SFS file should not be recognized as SFS file");
}
END_TEST

START_TEST(CreateSFS) {
    char *filePath = TEST_FILE;

    ChunkedFile *cf = sfs_createChunkedFile(filePath);

    fail_if(cf == NULL, "System should have created chunked file");
    fail_if(cf->location == NULL, "System should have set file location");
    
    fail_if(access(filePath, F_OK));
}
END_TEST

START_TEST(AddsSignatureToNewFile) {
    char *filePath = TEST_FILE;
    fail_if(sfs_checkIsSFS(filePath) != 1, "Newly-created file should be an SFS");
}
END_TEST

START_TEST(AddsMessageToFile) {
    char *filePath = "testOutput/testFileMessage";
    char msg[5] = {'h', 'e', 'l', 'l', 'o'};

    ChunkedFile *cf = sfs_createChunkedFile(filePath);

    sfs_setMessage(cf, msg, 5);

    sfs_getMessage(cf);

    fail_if(cf->message == NULL, "System should have read in message");
    fail_if(memcmp(msg, cf->message, sizeof(msg)) != 0, "Expected message not found");

}
END_TEST

START_TEST(OverwriteMessageInFile) {
    char *filePath = "testOutput/testFileMessage";
    char msg[5] = {'h', 'e', 'l', 'l', 'o'};
    char *updated = "NewMessage";

    ChunkedFile *cf = sfs_createChunkedFile(filePath);

    sfs_setMessage(cf, msg, 5);
    sfs_getMessage(cf);

    sfs_setMessage(cf, updated, 10);
    sfs_getMessage(cf);

    fail_if(cf->message == NULL, "System should have read in message");
    fail_if(memcmp(updated, cf->message, sizeof(updated)) != 0, "Expected message not found");

    fail_if(sfs_checkIsSFS(filePath) != 1, "Updated file should still be an SFS");

}
END_TEST

START_TEST(OpenChunkedFile) {

    char *filePath = "resource/indexedFile";
    ChunkedFile *cf = sfs_openChunkedFile(filePath);

    fail_if(cf == NULL);

}
END_TEST

START_TEST(ReadChunkFromFile) {

    char *filePath = "resource/indexedFile";
    ChunkedFile *cf = sfs_openChunkedFile(filePath);

    char *expectedData = "this is a test";

    //  This should read in the FAT table from the existing SFS
    char *chunkData = sfs_readChunk(cf, 1);

    fail_if(chunkData == NULL, "System did not read in chunk data");

    fail_unless(memcmp(expectedData, chunkData, 14) == 0, "Expected data were not found");

}
END_TEST

START_TEST(WriteChunkToFile) {

    char *filePath = "testOutput/writeChunkToFile";
    ChunkedFile *cf = sfs_createChunkedFile(filePath);
    cf->unitSize = 50;  //  Shorten unit size to make it easier to test

    char *expectedData = "writeCHUNK";

    sfs_writeChunk(cf, 0, "writeCHUNK", 10);

    cf = sfs_openChunkedFile(filePath);
    cf->unitSize = 50;  //  Shorten unit size to make it easier to test

    char *readIn = sfs_readChunk(cf, 0);

    fail_unless(memcmp(expectedData, readIn, 10) == 0, "Expected chunk data not found");

}
END_TEST

START_TEST(WriteChunkToSecondSegmentInFile) {

    char *filePath = "testOutput/writeSecondChunkToFile";
    ChunkedFile *cf = sfs_createChunkedFile(filePath);
    cf->unitSize = 50;  //  Shorten unit size to make it easier to test

    char *expectedData = "writeCHUNK";

    sfs_writeChunk(cf, 1, "writeCHUNK", 10);

    cf = sfs_openChunkedFile(filePath);
    cf->unitSize = 50;  //  Shorten unit size to make it easier to test

    char *readIn = sfs_readChunk(cf, 1);

    fail_unless(memcmp(expectedData, readIn, 10) == 0, "Failed to read chunk from segment beyond first segment");

}
END_TEST

START_TEST(WriteChunkToThirdSegmentInFile) {

    char *filePath = "testOutput/writeThirdChunkToFile";
    ChunkedFile *cf = sfs_createChunkedFile(filePath);
    cf->unitSize = 50;  //  Shorten unit size to make it easier to test

    char *expectedData = "writeCHUNK";

    sfs_writeChunk(cf, 2, "writeCHUNK", 10);

    cf = sfs_openChunkedFile(filePath);
    cf->unitSize = 50;  //  Shorten unit size to make it easier to test

    char *readIn = sfs_readChunk(cf, 2);

    fail_unless(memcmp(expectedData, readIn, 10) == 0, "Failed to read chunk from segment beyond first segment");

}
END_TEST

START_TEST(WriteChunksToSecondAndThirdSegmentInFile) {

    char *filePath = "testOutput/writeSecondAndThirdChunkToFile";
    ChunkedFile *cf = sfs_createChunkedFile(filePath);
    cf->unitSize = 50;  //  Shorten unit size to make it easier to test

    char *expectedData = "writeCHUNK";

    sfs_writeChunk(cf, 1, "Intermediary Chunk of Greatness", strlen("Intermediary Chunk of Greatness"));
    sfs_writeChunk(cf, 2, "writeCHUNK", 10);

    cf = sfs_openChunkedFile(filePath);
    cf->unitSize = 50;  //  Shorten unit size to make it easier to test

    char *readIn = sfs_readChunk(cf, 2);

    fail_unless(memcmp(expectedData, readIn, 10) == 0, "Failed to read chunk from segment beyond first segment");

    char *secondChunk = sfs_readChunk(cf, 1);
    fail_if(memcmp(secondChunk, "Intermediary Chunk of Greatness", strlen("Intermediary Chunk of Greatness")) != 0);

}
END_TEST

int main(void)
{
    Suite *suite = suite_create("Chunked File Management");
    SRunner *runner = srunner_create(suite);

    TCase *case1 = tcase_create("Basic Open");
    tcase_add_test(case1, LoadAFile);
    tcase_add_test(case1, NOT_AN_SFS);
    suite_add_tcase(suite, case1);

    TCase *createOpen = tcase_create("Create/Open");

    TCase *chunks = tcase_create("Chunk Tests");

    tcase_add_test(createOpen, CreateSFS);
    tcase_add_test(createOpen, AddsSignatureToNewFile);
    tcase_add_test(createOpen, AddsMessageToFile);
    tcase_add_test(createOpen, OverwriteMessageInFile);
    tcase_add_test(createOpen, OpenChunkedFile);
    tcase_add_test(chunks, ReadChunkFromFile);
    tcase_add_test(chunks, WriteChunkToFile);
    tcase_add_test(chunks, WriteChunkToSecondSegmentInFile);
    tcase_add_test(chunks, WriteChunkToThirdSegmentInFile);
    tcase_add_test(chunks, WriteChunksToSecondAndThirdSegmentInFile);
    suite_add_tcase(suite, createOpen);
    suite_add_tcase(suite, chunks);

    srunner_run_all(runner, CK_ENV);

    int failCount = srunner_ntests_failed(runner);

    return failCount == 0 ? 0 : -1;
}
