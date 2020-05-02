SOURCE_DIR = ./src
MAIN_FILE = $(SOURCE_DIR)/sfs_c.c
TEST_DIR = ./test
OUTPUT_DIR = ./built
OUTPUT_FILE = $(OUTPUT_DIR)/sfs_c.o

.PHONY: clean build buildTest

build: clean
	mkdir $(OUTPUT_DIR)
	gcc -Wall -o $(OUTPUT_FILE) $(MAIN_FILE)

clean:
	-rm -rf $(OUTPUT_DIR)

