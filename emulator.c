#include <stdio.h>
#include <stdlib.h>
#include "disassembler.c"

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
	int length;
	readFile(argv[1], &buffer, &length);

	printBytes(buffer, length);

	free(buffer);
	return 0;
}
