CC=gcc
CFLAGS=-I. -g -Wsign-conversion -pthread -lncurses
DEPS = util.h disassembler.h state.h instructions.h 
OBJ = util.o disassembler.o state.o instructions.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: chip8 test

chip8: $(OBJ) emulator.o
	$(CC) -o $@ $^ $(CFLAGS)

test: $(OBJ) unittest.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o test chip8
