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
PERF_TEST = $(TEST_DIR)/memCheck.c
PERF_EXE = ./memCheck

# Outputs
OUTPUT_DIR = ./built
OUTPUT_FILE = $(OUTPUT_DIR)/sfs_c_main

.PHONY: clean build test memCheck

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

memCheck: build
	gcc $(PERF_TEST) -Wall -g -o $(PERF_EXE) $(OBJ_COMPILE) -lcheck -lm -lpthread -lrt  -lm -lsubunit
	@mkdir $(TEST_OUT)
	valgrind -s --leak-check=yes --track-origins=yes $(PERF_EXE)