/*
SFS In C Project:
Internal functions (code that isn't exposed to outside world)
*/

#include <check.h>
#include <string.h>
#include <stdlib.h>

char* longToBytes(long long value) {
    char* longBytes = malloc(sizeof(char) * 8);
    
    int shiftFactor;
    int i=0;
    for(i=0; i<8; i++) {

        shiftFactor = 56 - (i * 8);

        if(i == 0) {
            longBytes[i] = (unsigned char)((value >> shiftFactor) & 0xFF);
        } else {
            longBytes[i] = (unsigned char)(value >> shiftFactor & 0xFF);
        }
    }

    return longBytes;
}

long long bytesToLong(char *bytes) {
    int i=0;
    //  From bytes
    long long sum = 0;
    
    int returnShiftFac;
    for(i=0; i<8; i++) {

        returnShiftFac = 56 - (i * 8);

        long rawByteData = (long long)((unsigned char)(bytes[i])) << returnShiftFac;
        sum |= rawByteData;
    }

    return sum;
}

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
