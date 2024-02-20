CC=gcc
LDFLAGS=
BIN=../bin

all: test_priority_queue

test_priority_queue: test_priority_queue.c ../src/priority_queue.c
	$(CC) -D_GNU_SOURCE test_priority_queue.c ../src/priority_queue.c -o $(BIN)/test_priority_queue

.PHONY: clean

clean:
	rm -f $(BIN)/*

