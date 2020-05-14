# sfs_c
A C library for reading files that use my Secure File System (chunked file) format

## IMPORTANT
This library is still under heavy development.  It should not be used for any applications until that development is complete.  There are several outstanding tasks such as initialization vector generation, key derivation, hashing, etc. that need to be completed first.

# Basic Requirements
You'll need to have valgrind and check installed in order to run the unit and mem leak tests.

# Building
To build clone the project and type ```make```

# SFS Specification Basics
## Chunk Format
A chunk must begin with the following two control bytes:

```1```:    The "Start of Medium" byte

```17```:   The length indicator byte

Following the length indicator byte 8 bytes must be provided to allocate a value of type ```long long``` (a long in Java), representing the length of the chunk's payload, in bytes

After the 8 bytes for length the next byte must be:

```2```:    The "Start of Content" byte

After the start of content byte must follow the chunk's payload
