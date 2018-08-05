CC=gcc
CFLAGS=-I. -g
DEPS = disassembler.h state.h instructions.h util.h
OBJ = emulator.o disassembler.o state.o instructions.o util.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

emulator: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o emulator
