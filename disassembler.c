#include <stdio.h>
#include <stdlib.h>
#include "state.c"

void readFile(char *filename, char **buffer, int *length)
{
	FILE *f = fopen(filename, "r");

	fseek(f, 0, SEEK_END);
	*length = ftell(f);
	fseek(f, 0, SEEK_SET);

	*buffer = malloc(*length);
	fread(*buffer, *length, 1, f);
	fclose(f);
}

void printBytes(char *buffer, int length)
{
	for (int i = 0; i < length; i++)
	{
		printf("%hhx ", buffer[i]);
	}

	printf("%c", '\n');
}
