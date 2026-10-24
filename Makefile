CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -O2 -Iinclude
SRC     = src/common.c src/student_avl.c src/library_hash.c \
          src/campus_graph.c src/helpdesk_heap.c src/algo_lab.c src/seed_data.c
OBJ     = $(SRC:.c=.o)

all: campushub

campushub: $(OBJ) src/main.o
	$(CC) $(CFLAGS) -o $@ $^

test_runner: $(OBJ) tests/test_all.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: test_runner
	./test_runner

bench: campushub
	./campushub --bench | tee docs/benchmark.csv

run: campushub
	./campushub

clean:
	rm -f src/*.o tests/*.o campushub test_runner campushub.exe test_runner.exe

.PHONY: all test bench run clean
