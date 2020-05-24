#include "sfs_crypt.h"
#include <gcrypt.h>
#include "sfs_about.h"
#include "../dependencies/log.c/src/log.h"
#include "sfs_util.c"
#include <math.h>

//  Initialization vectors are always 16 bytes long!
#define IV_LEN 16

#define SECMEM_ALLOCATED 1000000
#define GRYPT_VERSION "1.8.1"
#define LOG_CTX "SFS_GCRY\t%s"

#ifdef SFS_DEBUG
#define LOG_LEVEL 0 //  Trace level
#else
#define LOG_LEVEL 2 //  Info or above
#endif

#define AES 1
#define TWF 2

int sfs_startup() {
    const char *v = gcry_check_version(GRYPT_VERSION);
    log_set_level(LOG_LEVEL);

    //  Initialize secure memory
    gcry_error_t error;
    error = gcry_control(GCRYCTL_INIT_SECMEM, SECMEM_ALLOCATED, 0);
    if(error != 0) {
        log_fatal(LOG_CTX, gcry_strerror(error));
        abort();
    }

    error = gcry_control (GCRYCTL_INITIALIZATION_FINISHED, 0);
    if(error != 0) {
        log_fatal(LOG_CTX, gcry_strerror(error));
        abort();
    }

    printf("%s\n%s\n", SFS_C_ABOUT, SFS_C_VERSION);
    printf("libgcrypt Version Check:  %s\n", v);

    if(!v) {
        return 9000;
    }

    return 0;
}

/**
 * Perform key stretching/derivation to convert the given password into a key for encryption for AES
 */
static void *derivePasswordAES(char *password) {
    gcry_error_t err;
    gcry_md_hd_t digest;
    err = gcry_md_open(&digest, GCRY_MD_SHA256, GCRY_MD_FLAG_SECURE);
    if (err != 0){
        log_fatal("AES Key Derivation:  %s\n", gcry_strerror(err));
        return NULL;
    }

    //  Add additional algorithm, SHA3:
    err = gcry_md_enable(digest, GCRY_MD_SHA3_256);
    
    if (err != 0){
        log_fatal("AES Key Derivation:  %s\n", gcry_strerror(err));
        return NULL;
    }

    gcry_md_write(digest, password, strlen(password));

    void *hashed = gcry_md_read(digest, GCRY_MD_SHA256);

    //  Now derive key
    void *generatedKey = gcry_malloc_secure(32);
    char *salt = "abcdefghijklmno"; //  TODO    Generate
    err = gcry_kdf_derive(hashed, 32, GCRY_KDF_SCRYPT, 100, salt, strlen(salt), 10, 32, generatedKey);
    
    if (err != 0){
        log_fatal("AES Key Derivation:  %s\n", gcry_strerror(err));
        return NULL;
    }

    gcry_md_close(digest);

    return generatedKey;
}

/**
 * Perform key stretching/derivation to convert the given password into a key for encryption for AES
 */
static void *derivePasswordTwoFish(char *password) {
    gcry_error_t err;
    gcry_md_hd_t digest;
    err = gcry_md_open(&digest, GCRY_MD_SHA256, GCRY_MD_FLAG_SECURE);
    if (err != 0){
        log_fatal("TwoFish Key Derivation:  %s\n", gcry_strerror(err));
        return NULL;
    }

    //  Add additional algorithm, SHA3:
    err = gcry_md_enable(digest, GCRY_MD_SHA3_256);
    
    if (err != 0){
        log_fatal("TwoFish Key Derivation:  %s\n", gcry_strerror(err));
        return NULL;
    }

    gcry_md_write(digest, password, strlen(password));

    void *hashed = gcry_md_read(digest, GCRY_MD_SHA256);

    //  Now derive key
    void *generatedKey = gcry_malloc_secure(32);
    char *salt = "abcdefghijklmno"; //  TODO    Generate
    err = gcry_kdf_derive(hashed, 32, GCRY_KDF_SCRYPT, 100, salt, strlen(salt), 4, 32, generatedKey);
    
    if (err != 0){
        log_fatal("TwoFish Key Derivation:  %s\n", gcry_strerror(err));
        return NULL;
    }

    gcry_md_close(digest);

    return generatedKey;
}

static int calculateOutputLengthNeeded(int cipherType, int length) {

    int gcry_cipherType;
    if(cipherType == AES){
        gcry_cipherType = GCRY_CIPHER_AES256;
    } else {
        gcry_cipherType = GCRY_CIPHER_TWOFISH;
    }

    int blockLength = gcry_cipher_get_algo_blklen(gcry_cipherType);

    if(LOG_LEVEL == 0) {
        char *cipherName = "AES";
        if(cipherType == TWF) {
            cipherName = "TwoFish";
        }
        log_trace("Block length for %s=%d", cipherName, blockLength);
    }
    double numBlocksApprox = (double)length / (double)blockLength;
    int blockMultiplier = (int)ceil(numBlocksApprox);

    int outputLengthNeeded = blockLength * blockMultiplier;
    log_trace("Padding Needed:\torig=%d, approxBlocks=%lf, finalLen:%d", length, numBlocksApprox, outputLengthNeeded);

    return outputLengthNeeded;
}

static char* doEncrypt(int cipherType, char *data, char *password, int length) {

    int outputLengthNeeded = calculateOutputLengthNeeded(cipherType, length);

    void *key;
    if(cipherType == AES) {
        key = derivePasswordAES(password);
    } else {
        key = derivePasswordTwoFish(password);
    }

    int gcry_cipherType;
    if(cipherType == AES) {
        gcry_cipherType = GCRY_CIPHER_AES256;
    } else {
        gcry_cipherType = GCRY_CIPHER_TWOFISH;
    }

    //  Based on code found here
    //  https://cboard.cprogramming.com/c-programming/105743-how-decrypt-encrypt-using-libgcrypt-arc4.html#post937372
    gcry_error_t error;
    char * txtBuffer = data;
    char * encBuffer = calloc(outputLengthNeeded, sizeof(char));

    //  See also https://gnupg.org/documentation/manuals/gcrypt/Working-with-cipher-handles.html#Working-with-cipher-handles
    gcry_cipher_hd_t handle;
    error = gcry_cipher_open(&handle, gcry_cipherType, GCRY_CIPHER_MODE_CBC, GCRY_CIPHER_SECURE);
    size_t keyLength = gcry_cipher_get_algo_keylen(gcry_cipherType);
    error = gcry_cipher_setkey(handle, key, keyLength);

    //  See also https://www.gnupg.org/(es)/documentation/manuals/gcrypt/Random-Numbers.html#Random-Numbers
    unsigned char *iv = gcry_random_bytes_secure(IV_LEN, GCRY_VERY_STRONG_RANDOM);

    error = gcry_cipher_setiv(handle, iv, 16);
    error = gcry_cipher_encrypt(handle, encBuffer, outputLengthNeeded, txtBuffer, outputLengthNeeded);

    char *finalProduct = malloc(outputLengthNeeded + IV_LEN);
    memcpy(&finalProduct[0], iv, IV_LEN);
    memcpy(&finalProduct[IV_LEN], &encBuffer[0], outputLengthNeeded);
    
    gcry_cipher_close(handle);

    free(encBuffer);

    return finalProduct;
}

char * sfs_encrypt(char *data, char *password, int length){
    sfs_bytes_debug("B4Encrypt PLAIN", data, length, 0);

    int expectedLengthNeeded = calculateOutputLengthNeeded(TWF, length);

    char *memAddr;

    char * temp = doEncrypt(TWF, data, password, length);
    memAddr = temp;

    sfs_bytes_debug("TwoFish Encrypted", temp, expectedLengthNeeded+IV_LEN, 0);
    temp = doEncrypt(AES, temp, password, expectedLengthNeeded + IV_LEN);

    free(memAddr);

    sfs_bytes_debug("AES Encrypted", temp, expectedLengthNeeded+IV_LEN+IV_LEN, 0);
    return temp;
}

static char* doDecrypt(int cipherType, char *cipherText, char *password, int length) {

    void *key;
    if(cipherType == AES) {
        key = derivePasswordAES(password);
    } else {
        key = derivePasswordTwoFish(password);
    }

    int gcry_cipherType;
    if(cipherType == AES) {
        gcry_cipherType = GCRY_CIPHER_AES256;
    } else {
        gcry_cipherType = GCRY_CIPHER_TWOFISH;
    }

    int outputLengthNeeded = calculateOutputLengthNeeded(cipherType, length);
    
    gcry_error_t error;

    char * outBuffer = calloc(outputLengthNeeded, sizeof(char));

    //  See also https://gnupg.org/documentation/manuals/gcrypt/Working-with-cipher-handles.html#Working-with-cipher-handles
    gcry_cipher_hd_t handle;
    error = gcry_cipher_open(&handle, gcry_cipherType, GCRY_CIPHER_MODE_CBC, GCRY_CIPHER_SECURE);
    size_t keyLength = gcry_cipher_get_algo_keylen(gcry_cipherType);

    error = gcry_cipher_setkey(handle, key, keyLength);
    if(error != 0) {
        log_error("decry: setKey Error:\t%s", gcry_strerror(error));
    }

    char *iv = malloc(IV_LEN);
    memcpy(iv, cipherText, IV_LEN);
    error = gcry_cipher_setiv(handle, iv, 16);
    if(error != 0) {
        log_error("decry: setIV Error:\t%s", gcry_strerror(error));
    }

    char *cipherTextProper = &cipherText[IV_LEN];

    error = gcry_cipher_decrypt(handle, outBuffer, outputLengthNeeded, cipherTextProper, outputLengthNeeded);
    if(error != 0) {
        log_error("decry: Decrypt Error:\t%s", gcry_strerror(error));
    }

    gcry_cipher_close(handle);
    free(iv);

    return outBuffer;
}

char * sfs_decrypt(char *cipherText, char *password, int length) {

    int expectedLengthNeeded = calculateOutputLengthNeeded(TWF, length);

    char *memAddr;

    char *temp = doDecrypt(AES, cipherText, password, expectedLengthNeeded + IV_LEN);   //  (IV for AES + IV for TwoFish)
    memAddr = temp;

    sfs_bytes_debug("AES Decrypted", temp, expectedLengthNeeded+IV_LEN, 0);
    temp = doDecrypt(TWF, temp, password, length);

    free(memAddr);

    sfs_bytes_debug("TwoFish Decrypted", temp, length, 0);
    return temp;
}