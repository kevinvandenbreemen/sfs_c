#include <check.h>

START_TEST(LoadAFile) {

}
END_TEST

int main(void)
{
    Suite *suite = suite_create("File Loading");
    SRunner *runner = srunner_create(suite);


    TCase *case1 = tcase_create("Basic Open");
    tcase_add_test(case1, LoadAFile);
    suite_add_tcase(suite, case1);

    srunner_run_all(runner, CK_ENV);

    int failCount = srunner_ntests_failed(runner);

    return failCount == 0 ? 0 : -1;
}
