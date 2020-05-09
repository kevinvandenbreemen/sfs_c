/*
SFS In C Project:
Internal functions (code that isn't exposed to outside world)
Use this test file to unit test functions in the sfs_c.c file 
that are not implementations of the functions exposed in the 
header file (sfs_c.h) for this library
*/

#include <check.h>
#include <string.h>
#include <stdlib.h>
#include "../src/sfs_c.c"

START_TEST(LongToBytes) {

    //  To bytes
    long long lo = -221111111;
    char *longBytes = longToBytes(lo);

    long long sum = bytesToLong(longBytes);

    fail_if(sum != -221111111, "Value %ld != -221111111", sum);

}
END_TEST

int main(int argc, char const *argv[])
{
    Suite *suite = suite_create("Internal Functions");
    SRunner *runner = srunner_create(suite);

    TCase *case1 = tcase_create("Chunk Reading Functionality");
    tcase_add_test(case1, LongToBytes);
    suite_add_tcase(suite, case1);


    srunner_run_all(runner, CK_ENV);

    int failCount = srunner_ntests_failed(runner);
    return failCount == 0 ? 0 : -1;
}
