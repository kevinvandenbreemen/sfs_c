/*
SFS In C Project:
Internal functions (code that isn't exposed to outside world)
*/

#include <check.h>
#include <string.h>

START_TEST(LongToBytes) {

    //  To bytes
    long long lo = 42;
    char longBytes[8];
    
    int shiftFactor;
    int i=0;
    for(i=0; i<8; i++) {

        shiftFactor = 56 - (i * 8);

        if(i == 0) {
            longBytes[i] = (int)((lo >> shiftFactor) & 0xFF);
        } else {
            longBytes[i] = (int)(lo >> shiftFactor & 0xFF);
        }

        printf("Long bytes at %d = %d\n", i, longBytes[i]);
    }

    //  From bytes
    long long sum = 0;
    
    int returnShiftFac;
    for(i=0; i<8; i++) {

        returnShiftFac = 56 - (i * 8);

        long someLong = (long)( (longBytes[i] << returnShiftFac) );
        sum |= ((long)(longBytes[i]) << returnShiftFac);
        printf("sum=%ld\n", someLong);
    }

    fail_if(sum != 42, "Value %ld != 42", sum);

}
END_TEST

int main(int argc, char const *argv[])
{
    Suite *suite = suite_create("Internal Functions");
    SRunner *runner = srunner_create(suite);

    TCase *case1 = tcase_create("Basic Open");
    tcase_add_test(case1, LongToBytes);
    suite_add_tcase(suite, case1);


    srunner_run_all(runner, CK_ENV);

    int failCount = srunner_ntests_failed(runner);
    return failCount == 0 ? 0 : -1;
}
