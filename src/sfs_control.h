/*
SFS In C Project:
SFS Control Bytes
These bytes help the SFS detect beginnings/ends/etc in data stored on a file
*/

#ifndef IN_SFS_CTRL_C
#define IN_SFS_CTRL_C

/**
 * This byte signifies the premature end of a header in the space allocated
 * for that header.
 */
#define SFS_CTRL_END_OF_HEADER 4

#define START_OF_MEDIUM 1
#define LENGTH_IND 17
#define START_OF_CONTENT 2

#endif