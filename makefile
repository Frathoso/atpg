CC=gcc
CFLAGS=-Wall -g -lrt
SRC_DIR=./src
ATPG_LIBS_DIR=./src/libs
BIN_DIR=./bin
TARGET=atpg

atpgmake:
	$(CC) $(CFLAGS) $(SRC_DIR)/*.c $(ATPG_LIBS_DIR)/*.c -o $(BIN_DIR)/$(TARGET) 

clean:
	rm -f $(BIN_DIR)/*.o $(BIN_DIR)/$(TARGET)