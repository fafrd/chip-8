CC=gcc
CFLAGS=-I. -g -Wsign-conversion -pthread -lncurses
DEPS = util.h disassembler.h state.h instructions.h 
OBJ = util.o emulator.o disassembler.o state.o instructions.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

emulator: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o emulator
