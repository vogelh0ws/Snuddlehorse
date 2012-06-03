CC	= /usr/bin/gcc
CFLAGS	= -Wall -ggdb -std=c99
LDFLAGS = -lSDL

BIN	= snuddlehorse

all: snuddlehorse

snuddlehorse: pferd.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN) pferd.c

.PHONY: clean
clean:
	rm $(BIN)
