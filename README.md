# sfs_c
A C library for reading files that use my Secure File System (chunked file) format

## IMPORTANT
This library is still under heavy development.  It should not be used for any applications until that development is complete.  There are several outstanding tasks such as initialization vector generation, key derivation, hashing, etc. that need to be completed first.

# Contents

- [sfs_c](#sfs_c)
  - [IMPORTANT](#important)
- [Contents](#contents)
- [Basic Requirements](#basic-requirements)
- [Building](#building)
- [Testing](#testing)
  - [Test Case Files](#test-case-files)
    - [Unit Tests](#unit-tests)
      - [loaderTests.c](#loadertestsc)
      - [cryptoTests.c](#cryptotestsc)
      - [internalTests.c](#internaltestsc)
    - [Memory/Performance Tests](#memoryperformance-tests)
      - [memCheck.c](#memcheckc)
- [Debugging](#debugging)
  - [More details](#more-details)
  - [Debugging bytes](#debugging-bytes)
- [SFS Specification Basics](#sfs-specification-basics)
  - [Chunk Format](#chunk-format)


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
A basic sanity testing program that utilizes the library.  This program should be run using a memory checking tool like [valgrind](https://valgrind.org/).

# Debugging
You can enable debugging of the library by running ```make debugBuild```.

## More details
To enable debugging in the code you need to enable the ```SFS_DEBUG``` macro at compile time.  Simply add ```-DSFS_DEBUG``` to the gcc command line arguments in the makefile or run ```make debugBuild```.

Note that if you want to add debugging to anything in the makefile (for example, the ```memCheck``` build) update the task to call the debugBuild task rather than the build task.

## Debugging bytes
The sfs_util.h file provides a function called ```sfs_bytes_debug```, which you can use to print out the raw unsigned numerical values of bytes.  

Note that the function will skip large segments of repeated zeros and instead print the number of zeroes in parantheses, as in the example:

```
[1] [17] [0] [0]  ... (7) ... [14] [2] [116] [104] [105] [115] [32] [105] [115] [32] [97] [32] [116] [101] [115] [116] [0] [0]  ...(491495)
```

# SFS Specification Basics
## Chunk Format
A chunk must begin with the following two control bytes:

```1```:    The "Start of Medium" byte

```17```:   The length indicator byte

Following the length indicator byte 8 bytes must be provided to allocate a value of type ```long long``` (a long in Java), representing the length of the chunk's payload, in bytes

After the 8 bytes for length the next byte must be:

```2```:    The "Start of Content" byte

After the start of content byte must follow the chunk's payload
