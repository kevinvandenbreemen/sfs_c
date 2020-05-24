/*
SFS In C Project:
Unit tests to investigate using the libgcrypt library to satisfy the requirements
of the SFS crypto in C

Requires installation of:
libgcrypt20-dev
*/

#include <gcrypt.h>
#include <check.h>

#include "../src/sfs_crypt.h"
#define IV_LEN 16

START_TEST(InitGCrypt) {
    fail_if(sfs_startup() != 0);
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

    char * txtBuffer = "1234567890123456";
    char *cipherText = 
        sfs_encrypt(txtBuffer, "one test AES keyone test AES key", 16);
    sfs_bytes_debug("Plaintext",txtBuffer, 16, 0);

    printf("cipherText:  %s\n", cipherText);
    sfs_bytes_debug("CipherText\n", cipherText, 16+IV_LEN, 0);

    printf("Decrypting Now...\n");
    char *outBuffer = sfs_decrypt(cipherText, "one test AES keyone test AES key", 16);

    printf("OutBuff:  %s\n", outBuffer);
    sfs_bytes_debug("Decrypted",outBuffer, 16, 0);
    fail_if(memcmp(outBuffer, txtBuffer, strlen(txtBuffer)) != 0);

    
}
END_TEST

//  Cipher padding
START_TEST(TestEncryptsPlaintextOfNonOptimalLength) {
    char *plaintext = "hello world";    //  12 bytes (if you cound null char)

    char *cipherText = 
        sfs_encrypt(plaintext, "one test AES keyone test AES key", 11);

    printf("cipherText:  %s\n", cipherText);
    sfs_bytes_debug("CipherText\n", cipherText, 11+IV_LEN, 0);

    printf("Decrypting Now...\n");
    char *outBuffer = sfs_decrypt(cipherText, "one test AES keyone test AES key", 11);

    printf("OutBuff:  %s\n", outBuffer);
    sfs_bytes_debug("Decrypted",outBuffer, 11, 0);
    fail_if(memcmp(outBuffer, plaintext, 11) != 0);
}
END_TEST

START_TEST(TestEncryptStringFromMemoryTest) {

    char *plaintext = "HelloWorld To Encrypt";

    char *cipherText = 
        sfs_encrypt(plaintext, "one test AES keyone test AES key", strlen(plaintext));

    printf("cipherText:  %s\n", cipherText);
    sfs_bytes_debug("CipherText\n", cipherText, strlen(plaintext)+IV_LEN, 0);

    printf("Decrypting Now...\n");
    char *outBuffer = sfs_decrypt(cipherText, "one test AES keyone test AES key", strlen(plaintext));

    printf("OutBuff:  %s\n", outBuffer);
    sfs_bytes_debug("Decrypted",outBuffer, strlen(plaintext), 0);
    fail_if(memcmp(outBuffer, plaintext, strlen(plaintext)) != 0);
}
END_TEST

START_TEST(DoATwoFishEncrypt) {

    gcry_error_t error;
    
    size_t keyLength = gcry_cipher_get_algo_keylen(GCRY_CIPHER_TWOFISH);
    char *key = malloc(sizeof(char)*keyLength);
    char *password = "secret123";
    char *salt = "fasfasfasdfsf";

    error = gcry_kdf_derive(password, strlen(password), GCRY_KDF_SCRYPT, GCRY_KDF_SIMPLE_S2K, salt, strlen(salt), 100, keyLength, key);
    fail_if(error, gcry_strerror(error));
    printf("(%s)\tKey='%s'\n", password, key);

    //  Based on code found here
    //  https://cboard.cprogramming.com/c-programming/105743-how-decrypt-encrypt-using-libgcrypt-arc4.html#post937372
    

    char * txtBuffer = "123456789 abcdefghijklmnopqrstuvwzyz ABCDEFGHIJKLMNOPQRSTUVWZYZ";
    size_t txtLength = strlen(txtBuffer)+1; // string plus termination
    char * encBuffer = malloc(txtLength);
    char * outBuffer = malloc(txtLength);

    //  See also https://gnupg.org/documentation/manuals/gcrypt/Working-with-cipher-handles.html#Working-with-cipher-handles
    gcry_cipher_hd_t handle;
    error = gcry_cipher_open(&handle, GCRY_CIPHER_TWOFISH, GCRY_CIPHER_MODE_CBC, GCRY_CIPHER_SECURE);
    fail_if(error, gcry_strerror(error));

    
    

    error = gcry_cipher_setkey(handle, key, keyLength);
    fail_if(error, gcry_strerror(error));

    printf("Result of keyset:  %d\n", error);

    char *iv = "a test ini value";

    error = gcry_cipher_setiv(handle, iv, 16);
    fail_if(error, gcry_strerror(error));

    error = gcry_cipher_encrypt(handle, encBuffer, txtLength, txtBuffer, txtLength);
    fail_if(error, gcry_strerror(error));

    char *ivBytesIHope = malloc(16*sizeof(char));
    memcpy(ivBytesIHope, encBuffer, 16);
    printf("IV Bytes?  '%s'\n", ivBytesIHope);
    printf("Ciphertext:  '%s'\n", encBuffer);

    error = gcry_cipher_setiv(handle, iv, 16);
    fail_if(error, gcry_strerror(error));

    error = gcry_cipher_decrypt(handle, outBuffer, txtLength, encBuffer, txtLength);
    fail_if(error, gcry_strerror(error));

    printf("OutBuff:  %s\n", outBuffer);
    fail_if(memcmp(outBuffer, txtBuffer, txtLength) != 0);

    gcry_cipher_close(handle);

}
END_TEST

START_TEST(KeyGeneration) {

    char *key = "test";
    sfs_bytes_debug("Original Key ('test') bytes:", key, 4, 0);

//  Expected result from existing dual-layer encryption key derivation based on the text above
/*
key1:  [144] [165] [33] [30] [81] [178] [33] [236] [154] [223] [22] [165] [96] [136] [139] [118] [38] [139] [245] [98] [235] [141] [110] [112] [80] [78] [197] [90] [0] [30] [155] [160] 
key2:  [191] [154] [191] [253] [192] [113] [17] [195] [162] [78] [53] [94] [98] [64] [172] [206] [59] [10] [44] [47] [171] [77] [51] [100] [14] [100] [175] [239] [86] [229] [163] [4] 
*/

    gcry_error_t err;
    gcry_md_hd_t digest;
    err = gcry_md_open(&digest, GCRY_MD_SHA256, GCRY_MD_FLAG_SECURE);
    fail_if(err != 0, gcry_strerror(err));

    //  Add additional algorithm, SHA3:
    err = gcry_md_enable(digest, GCRY_MD_SHA3_256);
    fail_if(err != 0, gcry_strerror(err));

    gcry_md_write(digest, key, 4);

    void *hashed = gcry_md_read(digest, GCRY_MD_SHA256);

    sfs_bytes_debug("Hashed Key", hashed, 32, 0);

    //  Now derive key
    void *generatedKey = gcry_malloc_secure(32);
    char *salt = "abcdefghijklmno";
    err = gcry_kdf_derive(hashed, 32, GCRY_KDF_SCRYPT, GCRY_KDF_PBKDF2, salt, strlen(salt), 20, 32, generatedKey);
    
    fail_if(err != 0, gcry_strerror(err));

    sfs_bytes_debug("Generated Key", generatedKey, 32, 0);

    gcry_md_close(digest);

    sfs_bytes_debug("Generated Key (post close)", generatedKey, 32, 0);

}END_TEST

int main(int argc, char const *argv[])
{
    Suite *suite = suite_create("Crypto Stuff");
    SRunner *runner = srunner_create(suite);

    TCase *case1 = tcase_create("Cipher Fiddling Around");
    TCase *happy = tcase_create("Happy Paths");
    TCase *unhappy = tcase_create("Non-Happy Paths");

    tcase_add_test(happy, InitGCrypt);
    tcase_add_test(case1, OpenCipherHandle);
    tcase_add_test(happy, DoAESEncrypt);
    tcase_add_test(case1, DoATwoFishEncrypt);
    tcase_add_test(case1, KeyGeneration);
    suite_add_tcase(suite, happy);
    suite_add_tcase(suite, case1);
    

    tcase_add_test(unhappy, TestEncryptsPlaintextOfNonOptimalLength);
    tcase_add_test(unhappy, TestEncryptStringFromMemoryTest);
    suite_add_tcase(suite, unhappy);

    srunner_run_all(runner, CK_ENV);

    int failCount = srunner_ntests_failed(runner);
    return failCount == 0 ? 0 : -1;

    return 0;
}
