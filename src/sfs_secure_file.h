/*
SFS In C Project:
Secure File System implementation.
*/

#ifndef IN_SFS_C_SECURE_FILE
#define IN_SFS_C_SECURE_FILE

#include "sfs_chunkedfile.h"

typedef struct sfs_SecureFile {

    ChunkedFile *chunkedFile;
    char *password;

} sfs_SecureFile;

typedef enum {

    noSuchFile = 1,
    badPassword = 2,
    incorrectPassword = 3

} sfs_Error;

sfs_SecureFile *sfs_createSecureFile(char *filePath, char *password, sfs_Error *error);
sfs_SecureFile *sfs_openSecureFile(char *filePath, char *password, sfs_Error *error);

/**
 * Check if there has been an error.  If so returns a value other than NULL.  Otherwise returns NULL.  The returned
 * value is a string representing the error that has occurred
 */
char* sfs_checkError(sfs_Error *error);

#endif