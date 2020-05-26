#include <check.h>

#include "../src/sfs_secure_file.h"

START_TEST(CreateSecureFile) {
    printf("Placeholder\n");
}
END_TEST

int main(int argc, char const *argv[])
{
    
    Suite *suite = suite_create("Secure File Tests");
    SRunner *runner = srunner_create(suite);

    TCase *createOpen = tcase_create("Create/Open Secure File");
    suite_add_tcase(suite, createOpen);
    
    tcase_add_test(createOpen, CreateSecureFile);

    srunner_run_all(runner, CK_ENV);

    int failCount = srunner_ntests_failed(runner);
    return failCount == 0 ? 0 : -1;
}
