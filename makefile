CC=gcc
CFLAGS=-Wall -g
TARGET=main

all:
	$(CC) $(CFLAGS) *.c -o $(TARGET) 

clean:
	rm -f *.o $(TARGET)