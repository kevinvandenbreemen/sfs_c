#include <check.h>
#include <stdlib.h>
#include <stdio.h>

#include "../src/sfs_secure_file.h"

START_TEST(CreateSecureFile) {
    
    char *filePath = "testOutput/createSecureFile";
    sfs_Error *error = malloc(sizeof(sfs_Error));

    sfs_SecureFile *secFile = sfs_createSecureFile(filePath, "test", error);
    fail_if(sfs_checkError(error) != NULL);
    fail_if(secFile == NULL);

    free(error);

}
END_TEST

START_TEST(PreventCreatingSecureFileWithEmptyPassword) {
    char *filePath = "testOutput/badPassword";
    sfs_Error *error = malloc(sizeof(sfs_Error));

    sfs_SecureFile *secFile = sfs_createSecureFile(filePath, "", error);
    fail_unless(sfs_checkError(error) != NULL, "System should not allow password with 0 characters");
    fail_if(secFile != NULL, "System should not have created secure file");

    printf("Got error:  %s\n", sfs_checkError(error));
}
END_TEST

START_TEST(LoadAnExistingSecureFileSystem) {

    
    char *filePath = "testOutput/createOpenSecureFile";
    sfs_Error *error = malloc(sizeof(sfs_Error));

    sfs_SecureFile *secFile = sfs_createSecureFile(filePath, "test", error);
    fail_if(sfs_checkError(error) != NULL);

    //  Now open the file

    secFile = sfs_openSecureFile(filePath, "test", error);
    fail_if(sfs_checkError(error) != NULL, "Error:  %s", sfs_checkError(error));
    fail_if(secFile == NULL);


}
END_TEST

int main(int argc, char const *argv[])
{
    
    Suite *suite = suite_create("Secure File Tests");
    SRunner *runner = srunner_create(suite);

    TCase *createOpen = tcase_create("Create/Open Secure File");
    suite_add_tcase(suite, createOpen);
    
    tcase_add_test(createOpen, CreateSecureFile);
    tcase_add_test(createOpen, PreventCreatingSecureFileWithEmptyPassword);
    tcase_add_test(createOpen, LoadAnExistingSecureFileSystem);

    srunner_run_all(runner, CK_ENV);

    int failCount = srunner_ntests_failed(runner);
    return failCount == 0 ? 0 : -1;
}
