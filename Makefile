CC=g++
CFLAGS=-Wall #-D DEBUG
LDFLAGS=-lhidapi-hidraw -lsfml-graphics -lsfml-window -lsfml-system
OUTPUT=editor

.PHONY: all clean

all: src/simple_ui.o src/io.o src/hid.o
	$(CC) -o $(OUTPUT) $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

clean: 
	rm -rf src/*.o src/*.opp
