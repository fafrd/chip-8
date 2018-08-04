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
		if (i && i % 8 == 0)
			printf("\n");
		if (!(i % 8 == 0) && (i % 2 == 0))
			printf("\t");
		//printf("%hhx ", buffer[i]);
		char current_upper = buffer[i++];
		char current_lower = buffer[i];
		short current = (((short)current_upper) << 8) | (0x00ff & current_lower);
		printf("%hhx %hhx: %hx", current_upper, current_lower, current);
	}

	printf("%c", '\n');
}
