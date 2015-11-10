CC=gcc
CFLAGS=-std=c99
objs=output.o

.PHONY: all clean

all: mountgraph

mountgraph: mountgraph.c output.o
	$(CC) $(CFLAGS) -o $@ $^

output.o: output.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -f mountgraph $(objs)
