#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	char *filename = "pong.rom";
	FILE *f = fopen(filename, "r");

	fseek(f, 0, SEEK_END);
	int length = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *buffer = malloc(length);
	fread(buffer, length, 1, f);
	fclose(f);

	for (int i = 0; i < length; i++)
	{
		printf("%hhx ", buffer[i]);
	}

	printf("%c", '\n');
	free(buffer); // it's the right thing to do
}
