# sfs_c
A C library for reading files that use my Secure File System (chunked file) format

## IMPORTANT
This library is still under heavy development.  It should not be used for any applications until that development is complete.  There are several outstanding tasks such as initialization vector generation, key derivation, hashing, etc. that need to be completed first.

# Basic Requirements
You'll need to have valgrind and check installed in order to run the unit and mem leak tests.

# Building
To build clone the project and type ```make```

# Testing
## Test Case Files

You can test the library by typing ```make test``` to run the unit tests or ```make memCheck``` to run the memory tests.  Note that you'll need valgrind and check installed in order for this to run.

### Unit Tests
SFS_C uses the [check](https://libcheck.github.io/check/) library for unit testing.

The following unit test files are available to validate your code:

#### loaderTests.c
Tests of opening and reading/writing chunks to the indexed file

#### cryptoTests.c
Tests of any implementation of the sfs_crypt.h library

#### internalTests.c
Tests of internal utility functions

### Memory/Performance Tests
#### memCheck.c
A basic sanity testing program that utilizes the library.  This program should be run using a memory checking tool like ][valgrind](https://valgrind.org/).

# SFS Specification Basics
## Chunk Format
A chunk must begin with the following two control bytes:

```1```:    The "Start of Medium" byte

```17```:   The length indicator byte

Following the length indicator byte 8 bytes must be provided to allocate a value of type ```long long``` (a long in Java), representing the length of the chunk's payload, in bytes

After the 8 bytes for length the next byte must be:

```2```:    The "Start of Content" byte

After the start of content byte must follow the chunk's payload
