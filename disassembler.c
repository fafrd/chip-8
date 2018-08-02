#include <stdio.h>
#include <stdlib.h>

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

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("%s", "usage: disassemble file.rom\n");
		return -1;
	}

	char *buffer;
	int length;
	readFile(argv[1], &buffer, &length);

	printBytes(buffer, length);

	free(buffer); // it's the right thing to do
}
