CC=gcc
CFLAGS=-Wall #-D DEBUG
LDFLAGS=-lhidapi-hidraw
OUTPUT=editor

.PHONY: all clean

all: src/io.o src/hid.o
	$(CC) -o $(OUTPUT) $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean: 
	rm -rf src/*.o
