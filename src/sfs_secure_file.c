/*
SFS In C Project:
Secure File System implementation.
*/

#include <stdlib.h>
#include <string.h>
#include "sfs_secure_file.h"
#include "../dependencies/log.c/src/log.h"

sfs_SecureFile *sfs_createSecureFile(char *filePath, char *password, sfs_Error *error) {

    if(strlen(password) == 0) {
        log_error("Tried to create secure file system with blank password!");
        *error = badPassword;
        return NULL;
    }

    ChunkedFile *cf = sfs_createChunkedFile(filePath);
    sfs_SecureFile *secFile = malloc(sizeof(sfs_SecureFile));
    secFile->chunkedFile = cf;
    secFile->password = password;

    *error = 0;

    return secFile;

}

sfs_SecureFile *sfs_openSecureFile(char *filePath, char *password, sfs_Error *error) {

    return NULL;

}

/**
 * String representation of the error types
 */
static char* errorMessages[3] = {
    "No such file",
    "Bad password",
    "incorrect password"
};

char* sfs_checkError(sfs_Error *error) {
    if(*error >= 1 && *error <= 3) {
        return errorMessages[*error];
    }
    return 0;
}