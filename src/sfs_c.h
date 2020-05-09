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

    /**
     * File's message
     */
    char *message;

    /**
     * Number of bytes in file's message
     */
    int messageLength;

    /**
     * Unit size - the number of bytes to allocate inside
     * each chunk stored to the file
     */
    int unitSize;

} ChunkedFile;

/**
 * Create chunked file at the given location.  Note that if you call this function
 * with a file that already exists its contents will be overwritten!
 */
ChunkedFile *sfs_createChunkedFile(char *location);

/**
 * Load chunked file at the given location
 */
ChunkedFile *sfs_openChunkedFile(char *location);

/**
 * Read in the chunk at the given index of the file
 */
char *sfs_readChunk(ChunkedFile *chunkedFile, long long atIndex);

/**
 * Write in the given data as a chunk to the given file
 */
void sfs_writeChunk(ChunkedFile *chunkedFile, long long atIndex, char *data, int length);

/**
 * Set the message bytes on this chunked file
 */
void sfs_setMessage(ChunkedFile *cf, char *message, int length);

/**
 * Get the message at the beginning of the file
 */
void sfs_getMessage(ChunkedFile *cf);

#endif