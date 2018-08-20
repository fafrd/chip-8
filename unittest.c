#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 
#include <unistd.h>
#include <ncurses.h>
#include <termios.h>
#include "disassembler.h"
#include "state.h"
#include "instructions.h"
#include "util.h"

int main(int argc, char *argv[])
{
	initializeState();
	int result;

	printf("Running tests...\n");

	printf("testBranches()... ");
	result = testBranches();
	if (result != 0)
		printf("failure\n");
	else
		printf("success\n");

	printf("Testing complete.\n");
	return 0;
}

int testBranches()
{
	int rv = 0;

	// 3xnn
	r_pc = 0x400;
	

	// 4xnn

	// 5xy0

	// 9xy0

	// en9e

	// exa1


	return rv;
}
