CC=gcc
CFLAGS=-I. -g -Wsign-conversion -pthread -lncurses
DEPS = util.h disassembler.h state.h instructions.h 
OBJ = util.o disassembler.o state.o instructions.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: emulator test

emulator: $(OBJ) emulator.o
	$(CC) -o $@ $^ $(CFLAGS)

test: $(OBJ) unittest.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o emulator
