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
// either be difficult to test deterministically, or difficult
// to automate, or just more work than want to do right now:
// - cxnn random
// - ex9e skip if key pressed
// - exa1 skip if key not pressed
// - fx15 set delay timer
// - fx07 read delay timer
// - fx18 set sound timer
// - fx29 set register i to location of hex digit sprite
// - dxyn draw n length sprite at position vx, vy

int testBranches();
int testStores();
int testArithmetic();
int testJumps();
int testSubroutines();
int testRegisterI();
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
	// 6xnn
	initializeState();
	r_v3 = 0xcc;
	i_6xnn(0x3, 0xde);
	if (r_v3 != 0xde)
		return 1;

	// 8xy0
	initializeState();
	r_ve = 0x46;
	r_vc = 0x77;
	i_8xy0(0xe, 0xc);
	if (r_ve != 0x77)
		return 1;
	
	// 7xnn
	initializeState();
	r_v1 = 0x99;
	i_7xnn(0x1, 0x11);
	if (r_v1 != 0xaa)
		return 1;
	
	return 0;
}

int testArithmetic()
{
	// 8xy4
	initializeState();
	r_v1 = 0x90;
	r_v2 = 0x92;
	r_vf = 0xff;
	i_8xy4(0x1, 0x2);
	if (r_v1 != (0x90 + 0x92) % 0x100)
		return 1;
	if (r_vf != 0x1)
		return 1;

	initializeState();
	r_v1 = 0x90;
	r_v2 = 0x02;
	r_vf = 0xff;
	i_8xy4(0x1, 0x2);
	if (r_v1 != 0x92)
		return 1;
	if (r_vf != 0x0)
		return 1;

	// 8xy5
	initializeState();
	r_v1 = 0x90;
	r_v2 = 0x92;
	i_8xy5(0x1, 0x2);
	if (r_v1 != 0xfe)
		return 1;
	if (r_vf != 0x0)
		return 1;

	initializeState();
	r_v1 = 0x90;
	r_v2 = 0x82;
	i_8xy5(0x1, 0x2);
	if (r_v1 != 0x0e)
		return 1;
	if (r_vf != 0x1)
		return 1;

	// 8xy7
	initializeState();
	r_v1 = 0x90;
	r_v2 = 0x92;
	i_8xy7(0x1, 0x2);
	if (r_v1 != 0x02)
		return 1;
	if (r_vf != 0x1)
		return 1;

	initializeState();
	r_v1 = 0x92;
	r_v2 = 0x90;
	i_8xy7(0x1, 0x2);
	if (r_v1 != 0xfe)
		return 1;
	if (r_vf != 0x0)
		return 1;
	
	// 8xy2
	initializeState();
	r_v5 = 0b11001100;
	r_v6 = 0b10101010;
	i_8xy2(0x5, 0x6);
	if (r_v5 != 0b10001000)
		return 1;

	// 8xy1
	initializeState();
	r_v5 = 0b11001100;
	r_v6 = 0b10101010;
	i_8xy1(0x5, 0x6);
	if (r_v5 != 0b11101110)
		return 1;
	
	// 8xy3
	initializeState();
	r_v5 = 0b11001100;
	r_v6 = 0b10101010;
	i_8xy3(0x5, 0x6);
	if (r_v5 != 0b01100110)
		return 1;

	// 8xy6
	initializeState();
	r_v7 = 0b01010101;
	r_v8 = 0b11001100;
	r_vf = 0xff;
	i_8xy6(0x7, 0x8);
	if (r_v7 != 0b00101010)
		return 1;
	if (r_vf != 0x1)
		return 1;

	initializeState();
	r_v7 = 0b10101010;
	r_v8 = 0b11001100;
	r_vf = 0xff;
	i_8xy6(0x7, 0x8);
	if (r_v7 != 0b01010101)
		return 1;
	if (r_vf != 0x0)
		return 1;
	
	// 8xye
	initializeState();
	r_v7 = 0b01010101;
	r_v8 = 0b11001100;
	r_vf = 0xff;
	i_8xye(0x7, 0x8);
	if (r_v7 != 0b10101010)
		return 1;
	if (r_vf != 0x0)
		return 1;

	initializeState();
	r_v7 = 0b10101010;
	r_v8 = 0b11001100;
	r_vf = 0xff;
	i_8xye(0x7, 0x8);
	if (r_v7 != 0b01010100)
		return 1;
	if (r_vf != 0x1)
		return 1;
	
	return 0;
}

int testJumps()
{
	// 1xnn
	initializeState();
	r_v0 = 0x16;
	r_pc = 0x40;
	i_1nnn(0x132);
	// my jump instruction alreay expects loop()
	// to advance the pc by 2, so add 2 manually
	if (r_pc + 2 != 0x132)
		return 1;

	// bnnn
	initializeState();
	r_v0 = 0x16;
	r_pc = 0x40;
	i_bnnn(0x132);
	// my jump instruction alreay expects loop()
	// to advance the pc by 2, so add 2 manually
	if (r_pc + 2 != 0x148)
		return 1;
	
	return 0;
}

int testSubroutines()
{
	// I realize now that i coded the stack pointer a bit 
	// strangely; stack[0] will always be 0x0, because
	// i_2nnn will increment the stack pointer BEFORE pushing
	// to stack. Similarly, i_00ee decrements AFTER reading
	// from stack.
	// Emulation-wise, this is perfectly fine. ROMs cannot read
	// directly from the stack pointer anyways, it's all internal

	// 2nnn, 00ee
	initializeState();
	r_pc = 0x904;
	i_2nnn(0x124);
	// loop() will advance by 2, so decrement 2 here
	if (r_pc != 0x122)
		return 1;
	if (r_sp != 0x1)
		return 1;
	if (stack[1] != 0x904)
		return 1;

	i_2nnn(0x448);
	// loop() will advance by 2, so decrement 2 here
	if (r_pc != 0x446)
		return 1;
	if (r_sp != 0x2)
		return 1;
	if (stack[2] != 0x122)
		return 1;

	i_00ee();
	if (r_pc != 0x122)
		return 1;
	if (r_sp != 0x1)
		return 1;

	i_00ee();
	if (r_pc != 0x904)
		return 1;
	if (r_sp != 0x0)
		return 1;
	
	return 0;
}

int testRegisterI()
{
	// annn
	initializeState();
	r_i = 0xffff;
	i_annn(0x1777);
	if (r_i != 0x1777)
		return 1;
	
	// fx1e
	initializeState();
	r_i = 0x888;
	r_v2 = 0x22;
	i_fx1e(2);
	if (r_i != 0x8aa)
		return 1;
	
	return 0;
}

int testBCD()
{
	// fx33
	initializeState();
	r_i = 0x345;
	r_v6 = 0x1;
	i_fx33(0x6);
	if (mem[r_i] != 0)
		return 1;
	if (mem[r_i + 1] != 0)
		return 1;
	if (mem[r_i + 2] != 1)
		return 1;

	initializeState();
	r_i = 0x345;
	r_v6 = 0x19;
	i_fx33(0x6);
	if (mem[r_i] != 0)
		return 1;
	if (mem[r_i + 1] != 2)
		return 1;
	if (mem[r_i + 2] != 5)
		return 1;

	initializeState();
	r_i = 0x345;
	r_v6 = 0xeb;
	i_fx33(0x6);
	if (mem[r_i] != 2)
		return 1;
	if (mem[r_i + 1] != 3)
		return 1;
	if (mem[r_i + 2] != 5)
		return 1;

	initializeState();
	r_i = 0x345;
	r_v6 = 0xc8;
	i_fx33(0x6);
	if (mem[r_i] != 2)
		return 1;
	if (mem[r_i + 1] != 0)
		return 1;
	if (mem[r_i + 2] != 0)
		return 1;

	return 0;
}

int testSaveLoadAllRegisters()
{

	// fx55
	// fx65
	return -1;
	
	return 0;
}

