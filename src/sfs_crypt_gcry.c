#include "sfs_crypt.h"
#include <gcrypt.h>

//  Initialization vectors are always 16 bytes long!
#define IV_LEN 16

#define GRYPT_VERSION "1.8.1"

char * sfs_encrypt(char *data, char *password, int length){

    //  Based on code found here
    //  https://cboard.cprogramming.com/c-programming/105743-how-decrypt-encrypt-using-libgcrypt-arc4.html#post937372
    gcry_error_t error;

    char * txtBuffer = data;

    char * encBuffer = malloc(length);

    //  See also https://gnupg.org/documentation/manuals/gcrypt/Working-with-cipher-handles.html#Working-with-cipher-handles
    gcry_cipher_hd_t handle;
    error = gcry_cipher_open(&handle, GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_CBC, GCRY_CIPHER_SECURE);
    

    size_t keyLength = gcry_cipher_get_algo_keylen(GCRY_CIPHER_AES256);

    error = gcry_cipher_setkey(handle, password, keyLength);
    

    printf("Result of keyset:  %d\n", error);

    char *iv = "a test ini value";

    error = gcry_cipher_setiv(handle, iv, 16);
    
    error = gcry_cipher_encrypt(handle, encBuffer, length, txtBuffer, length);

    char *finalProduct = malloc(length + IV_LEN);
    memcpy(&finalProduct[0], iv, IV_LEN);
    printf("Will write %d to final product at %d\n", length, IV_LEN);
    memcpy(&finalProduct[IV_LEN], &encBuffer[0], length);
    
    gcry_cipher_close(handle);

    free(encBuffer);

    return finalProduct;
}

char * sfs_decrypt(char *cipherText, char *password, int length) {
    
    gcry_error_t error;

    char * outBuffer = malloc(length);

    //  See also https://gnupg.org/documentation/manuals/gcrypt/Working-with-cipher-handles.html#Working-with-cipher-handles
    gcry_cipher_hd_t handle;
    error = gcry_cipher_open(&handle, GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_CBC, GCRY_CIPHER_SECURE);
    size_t keyLength = gcry_cipher_get_algo_keylen(GCRY_CIPHER_AES256);

    error = gcry_cipher_setkey(handle, password, keyLength);
    printf("Result of keyset:  %d\n", error);

    char *iv = malloc(IV_LEN);
    memcpy(iv, cipherText, IV_LEN);
    error = gcry_cipher_setiv(handle, iv, 16);
    printf("Result of iv set (%s):  %d\n", iv, error);

    char *cipherTextProper = &cipherText[IV_LEN];

    error = gcry_cipher_decrypt(handle, outBuffer, length, cipherTextProper, length);
    printf("Result of decrypt:  %d\n", error);

    gcry_cipher_close(handle);

    return outBuffer;
}