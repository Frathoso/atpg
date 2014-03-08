CC = gcc
FILES = example_project.c test_suite_one.c test_suite_two.c ../lib/seatest.c
OUT_EXE = example

build: $(FILES)
	$(CC) -o $(OUT_EXE) $(FILES) -I ../lib

clean:
	rm -f *.o core

rebuild: clean build
