/*
SFS In C Project:
Unit tests to investigate using the libgcrypt library to satisfy the requirements
of the SFS crypto in C

Requires installation of:
libgcrypt20-dev
*/

#include <gcrypt.h>
#include <check.h>

START_TEST(OpenCipherHandle) {

    //  See also https://gnupg.org/documentation/manuals/gcrypt/Working-with-cipher-handles.html#Working-with-cipher-handles
    gcry_cipher_hd_t handle;
    gcry_cipher_open(&handle, GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_CBC, GCRY_CIPHER_SECURE);

    fail_if(handle == NULL, "System should have created cipher");

    gcry_cipher_close(handle);

}
END_TEST

START_TEST(SetCipherKey) {

    //  See also https://gnupg.org/documentation/manuals/gcrypt/Working-with-cipher-handles.html#Working-with-cipher-handles
    gcry_cipher_hd_t handle;
    gcry_cipher_open(&handle, GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_CBC, GCRY_CIPHER_SECURE);

    char key[32];
    printf("Result of keyset:  %d\n", gcry_cipher_setkey(handle, &key, 32));

    gcry_cipher_close(handle);

}
END_TEST

int main(int argc, char const *argv[])
{
    Suite *suite = suite_create("Crypto Stuff");
    SRunner *runner = srunner_create(suite);

    TCase *case1 = tcase_create("Cipher Fiddling Around");

    tcase_add_test(case1, OpenCipherHandle);
    tcase_add_test(case1, SetCipherKey);
    suite_add_tcase(suite, case1);

    srunner_run_all(runner, CK_ENV);

    int failCount = srunner_ntests_failed(runner);
    return failCount == 0 ? 0 : -1;

    return 0;
}
