# Inputs
SOURCE_DIR = ./src
MAIN_FILE = $(SOURCE_DIR)/sfs_c_main.c

# Testing
TEST_DIR = ./test
TEST_FILE = $(TEST_DIR)/loaderTests.c
TEST_EXE = ./runTests

# Outputs
OUTPUT_DIR = ./built
OUTPUT_FILE = $(OUTPUT_DIR)/sfs_c.o

.PHONY: clean build test

build: clean
	mkdir $(OUTPUT_DIR)
	gcc -Wall -o $(OUTPUT_FILE) $(MAIN_FILE)

clean:
	-rm -rf $(OUTPUT_DIR)

test: build
	gcc $(TEST_FILE) -Wall -o $(TEST_EXE) -lcheck -lm -lpthread -lrt  -lm -lsubunit
	./runTests
