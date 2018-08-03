#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disassembler.h"
#include "state.h"

// copies buffer to mem starting at mem[0x200]
// returns 0 on success
int loadRomToMemory(char **buffer, size_t length)
{
	if (length > 0xc8f)
		return 1; // rom too big
	memcpy((mem + 0x200), *buffer, length);
	return 0;
}

// main instruction processing loop
void loop()
{
	// program starts at 0x200
	r_pc = 0x200;
	char current_upper, current_lower;
	short current;

	while (1)
	{
		// set current instruction...
		// instructions are 2 bytes, get upper+lower byte, combine
		current_upper = mem[r_pc];
		current_lower = mem[r_pc+1];
		current = (((short)current_upper) << 8) | current_lower;

		// switch on first nibble (4 bits)
		switch (current_upper >> 4)
		{
			case 0x0:
			{
				if (current == 0x00e0)
					// TODO CLS - clear screen
				else if (current == 0x00ee)
					// TODO RET - return from subroutine
			}
			case 0x1:
			{
				short jumpDest = current & 0xfff;
				// TODO JP - jump
			}
			case 0x2:

			case 0x3:

			case 0x4:

			case 0x5:

			case 0x6:

			case 0x7:

			case 0x8:

			case 0x9:

			case 0xa:

			case 0xb:

			case 0xc:

			case 0xd:

			case 0xe:

			case 0xf:

		}

		// increment pc by 2 bytes
		r_pc += 2;
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("%s", "usage: emulator file.rom\n");
		return -1;
	}

	// Initialize registers, memory
	initializeState();

	// read romfile into buffer
	char *buffer;
	size_t length;
	readFile(argv[1], &buffer, &length);

	int loadSuccess = loadRomToMemory(&buffer, length);
	if (loadSuccess != 0)
	{
		printf("%s", "error: ROM size greater than 0xc8f bytes\n");
		return 1;
	}

	// start main program processing loop
	loop();

	//printBytes(buffer, length);

	free(buffer);
	return 0;
}
