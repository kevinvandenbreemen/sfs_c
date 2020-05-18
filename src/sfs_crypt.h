#ifndef IN_SFS_CRYPTO
#define IN_SFS_CRYPTO

/*
SFS In C Project:
Header file defining set of functions that any crypto that SFS relies on must provide
*/

/**
 * Perform any startup needed for the underlying cryptosystem to get ready.  Note that if you wish to print out
 * versioning info etc during your implementation's startup you should do so here as well.
 * 
 * Returns 0 if successful, any other value otherwise.
 */
int sfs_startup();

/**
 * Given the data and a password encrypts this data, returning the ciphertext
 * result.  Note that the resulting ciphertext will be prefixed with an initialization
 * vector.
 * 
 * length:  length of the data in bytes
 */
char * sfs_encrypt(char *data, char *password, int length);

/**
 * Given some ciphertext and a password, decrypts ciphertext, returning the resulting
 * plaintext.  
 * 
 * length:  Desired length of plaintext (do not include length of IV in this value)
 */
char * sfs_decrypt(char *cipherText, char *password, int length);

#endif