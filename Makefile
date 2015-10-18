CC=gcc
CFLAGS=-std=c99

.PHONY: all clean

all: mountgraph

mountgraph: mountgraph.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f mountgraph
