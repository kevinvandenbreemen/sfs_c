#include "sfs_crypt.h"
#include <gcrypt.h>

#define GRYPT_VERSION "1.8.1"

char * sfs_encrypt(char *data, char *password){

    //  Based on code found here
    //  https://cboard.cprogramming.com/c-programming/105743-how-decrypt-encrypt-using-libgcrypt-arc4.html#post937372
    gcry_error_t error;

    char * txtBuffer = data;
    size_t txtLength = strlen(txtBuffer)+1; // string plus termination
    char * encBuffer = malloc(txtLength);
    char * outBuffer = malloc(txtLength);

    //  See also https://gnupg.org/documentation/manuals/gcrypt/Working-with-cipher-handles.html#Working-with-cipher-handles
    gcry_cipher_hd_t handle;
    error = gcry_cipher_open(&handle, GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_CBC, GCRY_CIPHER_SECURE);
    

    size_t keyLength = gcry_cipher_get_algo_keylen(GCRY_CIPHER_AES256);

    error = gcry_cipher_setkey(handle, password, keyLength);
    

    printf("Result of keyset:  %d\n", error);

    char *iv = "a test ini value";

    error = gcry_cipher_setiv(handle, iv, 16);
    
    error = gcry_cipher_encrypt(handle, encBuffer, txtLength, txtBuffer, txtLength);
    
    gcry_cipher_close(handle);

    return encBuffer;
}

char * sfs_decrypt(char *cipherText, char *password) {
    
    gcry_error_t error;

    char * txtBuffer = "123456789 abcdefghijklmnopqrstuvwzyz ABCDEFGHIJKLMNOPQRSTUVWZYZ";
    size_t txtLength = strlen(txtBuffer)+1; // string plus termination
    char * encBuffer = malloc(txtLength);
    char * outBuffer = malloc(txtLength);

    //  See also https://gnupg.org/documentation/manuals/gcrypt/Working-with-cipher-handles.html#Working-with-cipher-handles
    gcry_cipher_hd_t handle;
    error = gcry_cipher_open(&handle, GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_CBC, GCRY_CIPHER_SECURE);
    size_t keyLength = gcry_cipher_get_algo_keylen(GCRY_CIPHER_AES256);

    error = gcry_cipher_setkey(handle, password, keyLength);
    printf("Result of keyset:  %d\n", error);

    char *iv = "a test ini value";
    error = gcry_cipher_setiv(handle, iv, 16);
    error = gcry_cipher_decrypt(handle, outBuffer, txtLength, cipherText, txtLength);
    printf("OutBuff:  %s\n", outBuffer);
    

    gcry_cipher_close(handle);

    return outBuffer;
}