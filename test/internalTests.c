/*
SFS In C Project:
Internal functions (code that isn't exposed to outside world)
*/

#include <check.h>
#include <string.h>

START_TEST(LongToBytes) {

    //  To bytes
    long long lo = -221111111;
    char longBytes[8];
    
    int shiftFactor;
    int i=0;
    for(i=0; i<8; i++) {

        shiftFactor = 56 - (i * 8);

        if(i == 0) {
            longBytes[i] = (unsigned char)((lo >> shiftFactor) & 0xFF);
        } else {
            longBytes[i] = (unsigned char)(lo >> shiftFactor & 0xFF);
        }

        printf("Long bytes at %d = %d\n", i, longBytes[i]);
    }

    //  From bytes
    long long sum = 0;
    
    int returnShiftFac;
    for(i=0; i<8; i++) {

        returnShiftFac = 56 - (i * 8);

        long rawByteData = (long long)((unsigned char)(longBytes[i])) << returnShiftFac;
        sum |= rawByteData;
        printf("longSegment=%ld\n", rawByteData);
    }

    fail_if(sum != -221111111, "Value %ld != -221111111", sum);

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
