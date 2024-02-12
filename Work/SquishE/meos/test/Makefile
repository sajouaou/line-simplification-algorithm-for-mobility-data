CC=gcc
CFLAGS=-I. -O2
DEPS = priority_queue.h
OBJ = test_priority_queue.o priority_queue.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test_priority_queue: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o test_priority_queue