/*
SFS In C Project:
Unit tests to investigate using the libgcrypt library to satisfy the requirements
of the SFS crypto in C

Requires installation of:
libgcrypt20-dev
*/

#include <gcrypt.h>
#include <check.h>

START_TEST(InitGCrypt) {
    //  This testing was done as of version 1.8.1 installed on my machine and also Github task runner
    const char *v = gcry_check_version("1.8.1");
    printf("Version Check:  %s\n", v);

    fail_if(!v);
}
END_TEST

START_TEST(OpenCipherHandle) {

    //  See also https://gnupg.org/documentation/manuals/gcrypt/Working-with-cipher-handles.html#Working-with-cipher-handles
    gcry_cipher_hd_t handle;
    gcry_cipher_open(&handle, GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_CBC, GCRY_CIPHER_SECURE);

    fail_if(handle == NULL, "System should have created cipher");

    gcry_cipher_close(handle);

}
END_TEST

START_TEST(DoAESEncrypt) {

    //  Based on code found here
    //  https://cboard.cprogramming.com/c-programming/105743-how-decrypt-encrypt-using-libgcrypt-arc4.html#post937372
    gcry_error_t error;

    char * txtBuffer = "123456789 abcdefghijklmnopqrstuvwzyz ABCDEFGHIJKLMNOPQRSTUVWZYZ";
    size_t txtLength = strlen(txtBuffer)+1; // string plus termination
    char * encBuffer = malloc(txtLength);
    char * outBuffer = malloc(txtLength);

    //  See also https://gnupg.org/documentation/manuals/gcrypt/Working-with-cipher-handles.html#Working-with-cipher-handles
    gcry_cipher_hd_t handle;
    error = gcry_cipher_open(&handle, GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_CBC, GCRY_CIPHER_SECURE);
    fail_if(error);

    size_t keyLength = gcry_cipher_get_algo_keylen(GCRY_CIPHER_AES256);
    char key[32] = "one test AES keyone test AES key";

    error = gcry_cipher_setkey(handle, key, keyLength);
    fail_if(error);

    printf("Result of keyset:  %d\n", error);

    char *iv = "a test ini value";

    error = gcry_cipher_setiv(handle, iv, 16);
    fail_if(error);

    error = gcry_cipher_encrypt(handle, encBuffer, txtLength, txtBuffer, txtLength);
    fail_if(error);

    error = gcry_cipher_setiv(handle, iv, 16);
    fail_if(error);

    error = gcry_cipher_decrypt(handle, outBuffer, txtLength, encBuffer, txtLength);
    fail_if(error);

    printf("OutBuff:  %s\n", outBuffer);
    fail_if(memcmp(outBuffer, txtBuffer, txtLength) != 0);

    gcry_cipher_close(handle);

}
END_TEST

START_TEST(DoATwoFishEncrypt) {

    //  Based on code found here
    //  https://cboard.cprogramming.com/c-programming/105743-how-decrypt-encrypt-using-libgcrypt-arc4.html#post937372
    gcry_error_t error;

    char * txtBuffer = "123456789 abcdefghijklmnopqrstuvwzyz ABCDEFGHIJKLMNOPQRSTUVWZYZ";
    size_t txtLength = strlen(txtBuffer)+1; // string plus termination
    char * encBuffer = malloc(txtLength);
    char * outBuffer = malloc(txtLength);

    //  See also https://gnupg.org/documentation/manuals/gcrypt/Working-with-cipher-handles.html#Working-with-cipher-handles
    gcry_cipher_hd_t handle;
    error = gcry_cipher_open(&handle, GCRY_CIPHER_TWOFISH, GCRY_CIPHER_MODE_CBC, GCRY_CIPHER_SECURE);
    fail_if(error);

    size_t keyLength = gcry_cipher_get_algo_keylen(GCRY_CIPHER_TWOFISH);
    char key[32] = "one test AES keyone test AES key";

    error = gcry_cipher_setkey(handle, key, keyLength);
    fail_if(error);

    printf("Result of keyset:  %d\n", error);

    char *iv = "a test ini value";

    error = gcry_cipher_setiv(handle, iv, 16);
    fail_if(error);

    error = gcry_cipher_encrypt(handle, encBuffer, txtLength, txtBuffer, txtLength);
    fail_if(error);

    error = gcry_cipher_setiv(handle, iv, 16);
    fail_if(error);

    error = gcry_cipher_decrypt(handle, outBuffer, txtLength, encBuffer, txtLength);
    fail_if(error);

    printf("OutBuff:  %s\n", outBuffer);
    fail_if(memcmp(outBuffer, txtBuffer, txtLength) != 0);

    gcry_cipher_close(handle);

}
END_TEST

int main(int argc, char const *argv[])
{
    Suite *suite = suite_create("Crypto Stuff");
    SRunner *runner = srunner_create(suite);

    TCase *case1 = tcase_create("Cipher Fiddling Around");

    tcase_add_test(case1, InitGCrypt);
    tcase_add_test(case1, OpenCipherHandle);
    tcase_add_test(case1, DoAESEncrypt);
    tcase_add_test(case1, DoATwoFishEncrypt);
    suite_add_tcase(suite, case1);

    srunner_run_all(runner, CK_ENV);

    int failCount = srunner_ntests_failed(runner);
    return failCount == 0 ? 0 : -1;

    return 0;
}
