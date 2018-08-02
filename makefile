CC=gcc
CFLAGS=-I.
DEPS = disassembler.h state.h
OBJ = emulator.o disassembler.o state.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

emulator: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o emulator
