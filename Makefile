# Inputs
SOURCE_DIR = ./src
MAIN_FILE = $(SOURCE_DIR)/sfs_c_main.c
OBJ_COMPILE = $(SOURCE_DIR)/sfs_c.c
OBJ_FILE = ./sfs_c.o

# Testing
TEST_DIR = ./test
TEST_FILE = $(TEST_DIR)/loaderTests.c
TEST_EXE = ./runTests
TEST_OUT = ./testOutput

# Outputs
OUTPUT_DIR = ./built
OUTPUT_FILE = $(OUTPUT_DIR)/sfs_c_main

.PHONY: clean build test

build: clean
	mkdir $(OUTPUT_DIR)
	gcc -Wall -o $(OBJ_FILE) -c $(OBJ_COMPILE)
	gcc -Wall -o $(OUTPUT_FILE) $(MAIN_FILE)
	$(OUTPUT_FILE)

clean:
	-rm -rf $(OUTPUT_DIR)
	-rm -rf $(TEST_OUT)

test: build
	gcc $(TEST_FILE) -Wall -o $(TEST_EXE) $(OBJ_FILE) -lcheck -lm -lpthread -lrt  -lm -lsubunit
	@mkdir $(TEST_OUT)
	./runTests
