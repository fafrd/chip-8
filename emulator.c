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
void loop(void *location)
{

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

	//printBytes(buffer, length);

	free(buffer);
	return 0;
}
