CC	= /usr/bin/gcc
CFLAGS	= -Wall -ggdb -std=c99 `sdl-config --cflags`
LDFLAGS = `sdl-config --libs`

BIN	= snuddlehorse

all: snuddlehorse

snuddlehorse: pferd.c
	$(CC) -o $(BIN) pferd.c $(CFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	rm $(BIN)
