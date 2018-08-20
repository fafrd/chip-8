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

// These tests could be more robust. right now I'm just
// testing the happy path. If I were to get really in depth, 
// I'd test all edge cases and would implement a way to 
// check that state is correct; that is, only the state that
// I want to be mutated has been mutated.
//
// Also I skipped the following instructions. They would 
// either be difficult to test deterministically or difficult
// to automate:
// - cxnn random
// - ex9e skip if key pressed
// - exa1 skip if key not pressed
// - fx15 set delay timer
// - fx07 read delay timer
// - fx18 set sound timer

int testBranches();
int testStores();
int testArithmetic();
int testJumps();
int testSubroutines();
int testRegisterI();
int testDraw();
int testBCD();
int testSaveLoadAllRegisters();

void printResult(int res)
{
	if (res != 0)
		printf("failure!\n");
	else
		printf("success.\n");

	// small pause to make it feel more realistic lol
	usleep(20000);
}

int main(int argc, char *argv[])
{
	initializeState();
	int result;

	printf("Running tests...\n\n");

	printf("testBranches()... ");
	printResult(testBranches());

	printf("testStores()... ");
	printResult(testStores());

	printf("testArithmetic()... ");
	printResult(testArithmetic());

	printf("testJumps()... ");
	printResult(testJumps());

	printf("testSubroutines()... ");
	printResult(testSubroutines());

	printf("testRegisterI()... ");
	printResult(testRegisterI());

	printf("testDraw()... ");
	printResult(testDraw());

	printf("testBCD())... ");
	printResult(testBCD());

	printf("testSaveLoadAllRegisters()... ");
	printResult(testSaveLoadAllRegisters());

	printf("\nTesting complete.\n");
	return 0;
}

int testBranches()
{
	// 3xnn
	initializeState();
	r_pc = 0x400;
	r_v5 = 0x23;
	i_3xnn(0x5, 0x24);
	if (r_pc != 0x400)
		return 1;

	initializeState();
	r_pc = 0x400;
	r_v5 = 0x23;
	i_3xnn(5, 0x23);
	if (r_pc == 0x400)
		return 1;

	// 4xnn
	initializeState();
	r_pc = 0x400;
	r_v5 = 0x23;
	i_4xnn(0x5, 0x24);
	if (r_pc == 0x400)
		return 1;

	initializeState();
	r_pc = 0x400;
	r_v5 = 0x23;
	i_4xnn(5, 0x23);
	if (r_pc != 0x400)
		return 1;

	// 5xy0
	initializeState();
	r_pc = 0x400;
	r_v7 = 0xfe;
	r_v8 = 0xff;
	i_5xy0(0x7, 0x8);
	if (r_pc != 0x400)
		return 1;

	initializeState();
	r_pc = 0x400;
	r_v7 = 0xfe;
	r_v8 = 0xfe;
	i_5xy0(0x7, 0x8);
	if (r_pc == 0x400)
		return 1;

	// 9xy0
	initializeState();
	r_pc = 0x400;
	r_v7 = 0xfe;
	r_v8 = 0xff;
	i_9xy0(0x7, 0x8);
	if (r_pc == 0x400)
		return 1;

	initializeState();
	r_pc = 0x400;
	r_v7 = 0xfe;
	r_v8 = 0xfe;
	i_9xy0(0x7, 0x8);
	if (r_pc != 0x400)
		return 1;

	// ex9e
	// manual testing required

	// exa1
	// manual testing required

	return 0;
}

int testStores()
{
	// 8xy0
	
	// 7xnn
	
	return 0;
}

int testArithmetic()
{
	// 8xy4
	// 8xy5
	// 8xy7
	// 8xy2
	// 8xy1
	// 8xy3
	// 8xy6
	// 8xye
}

int testJumps()
{
	// 1xnn
	// bnnn
}

int testSubroutines()
{
	// 2nnn
	// 00ee
	// 0nnn
}

int testRegisterI()
{
	// annn
	// fx1e
	// fx29
}

int testDraw()
{
	// dxyn
}

int testBCD()
{
	// fx33
}

int testSaveLoadAllRegisters()
{
	// TODO compare: some references implement these
	// instructions differently... figure out what the
	// differences are, exactly.

	// fx55
	// fx65
}

