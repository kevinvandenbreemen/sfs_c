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

int main(void)
{
    Suite *suite = suite_create("Chunked File Management");
    SRunner *runner = srunner_create(suite);


    TCase *case1 = tcase_create("Basic Open");
    tcase_add_test(case1, LoadAFile);
    tcase_add_test(case1, NOT_AN_SFS);
    suite_add_tcase(suite, case1);

    TCase *createOpen = tcase_create("Create/Open");
    tcase_add_test(createOpen, CreateSFS);
    tcase_add_test(createOpen, AddsSignatureToNewFile);
    tcase_add_test(createOpen, AddsMessageToFile);
    suite_add_tcase(suite, createOpen);

    srunner_run_all(runner, CK_ENV);

    int failCount = srunner_ntests_failed(runner);

    return failCount == 0 ? 0 : -1;
}
