#include <stdio.h>
#include <stdlib.h>
#include "disassembler.h"
#include "state.h"

void readFile(char *filename, char **buffer, size_t *length)
{
	FILE *f = fopen(filename, "r");

	fseek(f, 0, SEEK_END);
	*length = ftell(f);
	fseek(f, 0, SEEK_SET);

	*buffer = malloc(*length);
	fread(*buffer, *length, 1, f);
	fclose(f);
}

void printBytes(char *buffer, size_t length)
{
	for (int i = 0; i < length; i++)
	{
		printf("%hhx ", buffer[i]);
	}

	printf("%c", '\n');
}
