CC=gcc
CFLAGS=-Wall -g
TARGET=atpg

all:
	$(CC) $(CFLAGS) *.c -o $(TARGET) 

clean:
	rm -f *.o $(TARGET)