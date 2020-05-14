#ifndef IN_SFS_CRYPTO
#define IN_SFS_CRYPTO

/*
SFS In C Project:
Header file defining set of functions that any crypto that SFS relies on must provide
*/

/**
 * Given the data and a password encrypts this data, returning the ciphertext
 * result.
 */
char * sfs_encrypt(char *data, char *password);

/**
 * Given some ciphertext and a password, decrypts ciphertext, returning the resulting
 * plaintext.
 */
char * sfs_decrypt(char *cipherText, char *password);

#endif