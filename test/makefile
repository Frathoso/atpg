CC = gcc
BIN = ./bin
LIBS = ./libs
ATPG_SOURCES = ../src/libs/*.c
ATPG_SOURCES_LIBS = ../src/libs

FILES = *.c libs/seatest.c $(SOURCES) $(ATPG_SOURCES)
EXE = $(BIN)/test

build: $(FILES)
	$(CC) -o $(EXE) $(FILES) -I $(LIBS) -I $(ATPG_SOURCES_LIBS)

clean:
	rm -f *.o $(BIN)/test

rebuild: clean build
