# Dependencies/Artifacts
DEP_DIR = dependencies
DEP_OBJ_DIR = $(DEP_DIR)/bin
DEP_SRC_LOG = $(DEP_DIR)/log.c/src/log.c
DEP_BIN_LOG = $(DEP_OBJ_DIR)/log.o
DEP_ALL_OBJ_FILES = $(DEP_BIN_LOG) `libgcrypt-config --libs` -lm

# Inputs
SOURCE_DIR = ./src
MAIN_FILE = $(SOURCE_DIR)/sfs_c_main.c
OBJ_COMPILE = $(SOURCE_DIR)/sfs_c.c
CRY_COMPILE = $(SOURCE_DIR)/sfs_crypt_gcry.c
OBJ_FILE = ./sfs_c.o
OBJ_GCRY = ./sfs_c_gcry.o

# Testing
TEST_DIR = ./test
TEST_FILE = $(TEST_DIR)/loaderTests.c
TEST_EXE = ./runTests
TEST_OUT = ./testOutput
PERF_TEST = $(TEST_DIR)/memCheck.c
PERF_EXE = ./memCheck

# Run individual test cases.  Un-comment this to run specific testcase
#export CK_RUN_CASE=Non-Happy Paths

# Outputs
OUTPUT_DIR = ./built
OUTPUT_FILE = $(OUTPUT_DIR)/sfs_c_main

# Compilation
GCC_FLAGS =

.PHONY: clean build test memCheck dependencies enableDebug debugBuild

build: clean dependencies
	mkdir $(OUTPUT_DIR)
	gcc -Wall $(GCC_FLAGS) -o $(OBJ_FILE) -c $(OBJ_COMPILE) $(DEP_ALL_OBJ_FILES)
	gcc -Wall $(GCC_FLAGS) -o $(OUTPUT_FILE) $(MAIN_FILE) $(DEP_ALL_OBJ_FILES)

	$(OUTPUT_FILE)

enableDebug:
	$(eval GCC_FLAGS = -DSFS_DEBUG -g)

# Build the code with SFS_DEBUG macro enabled 
debugBuild: enableDebug build

# Pull in dependencies that the product requires
dependencies:
	-mkdir -p $(DEP_DIR)/log.c && mkdir $(DEP_OBJ_DIR)
	git clone https://github.com/rxi/log.c.git dependencies/log.c
	gcc -Wall -o $(DEP_BIN_LOG) -c $(DEP_SRC_LOG)

clean:
	-rm -rf $(OUTPUT_DIR)
	-rm -rf $(TEST_OUT)
	-rm -rf $(DEP_DIR)

test: debugBuild
	gcc $(TEST_FILE) -Wall -o $(TEST_EXE) $(OBJ_FILE) $(DEP_ALL_OBJ_FILES) -lcheck -lm -lpthread -lrt  -lm -lsubunit
	@mkdir $(TEST_OUT)
	./runTests

	gcc $(TEST_DIR)/internalTests.c -Wall -o $(TEST_EXE) $(DEP_ALL_OBJ_FILES) -lcheck -lm -lpthread -lrt  -lm -lsubunit
	./runTests

	# See also https://gnupg.org/documentation/manuals/gcrypt/Building-sources.html#Building-sources regarding libgcrypt-config command
	# and why it's needed in order for this to build
	gcc $(GCC_FLAGS) -g $(TEST_DIR)/cryptoTests.c -Wall -o $(TEST_EXE) $(DEP_ALL_OBJ_FILES) $(OBJ_FILE) -lcheck -lpthread -lrt  -lm -lsubunit
	./runTests

	gcc $(GCC_FLAGS) $(TEST_DIR)/secureFileTests.c -Wall -o $(TEST_EXE) $(OBJ_FILE) $(DEP_ALL_OBJ_FILES)  -lcheck -lpthread -lrt -lsubunit
	./runTests

memCheck: build
	gcc $(PERF_TEST) -Wall -g -o $(PERF_EXE) $(OBJ_COMPILE) $(DEP_ALL_OBJ_FILES) -lcheck -lpthread -lrt -lsubunit
	@mkdir $(TEST_OUT)
	valgrind --leak-check=yes --track-origins=yes $(PERF_EXE)