#ifndef IN_SFS_C
#define IN_SFS_C

#define SFS_C_VERISON "1.0.0x"

/*
 * Check if the file at the given path is a secure file system file.  If the file
 * is an SFS file the function will return 1.  Otherwise it will return a number other
 * than 1 if the file is not an SFS or an error has occurred.
 */
int sfs_checkIsSFS(char *filePath);

/**
 * Stores/reads "chunks" of bytes to and from a file
 */
typedef struct ChunkedFile {

    /**
     * path to the file on the file system
     */
    char *location;

} ChunkedFile;

/**
 * Load/create chunked file at the given location
 */
ChunkedFile *sfs_createChunkedFile(char *location);

/**
 * Set the message bytes on this chunked file
 */
void sfs_setMessage(ChunkedFile *cf, char *message, int length);

#endif