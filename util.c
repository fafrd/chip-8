#include <stdio.h>
#include <stdbool.h> 
#include <ncurses.h>
#include "state.h"

// prints contents of mem to screen for debugging
void dumpMem()
{
	for (int i = 0; i < 0x100; i++)
	{
		printf("%04hx\t", i*0x10);
		for (int j = 0; j < 0x10; j++)
		{
			printf("%02hhx ", mem[i*0x10 + j]);
			if (j == 0x7)
				printf(" ");
		}
		printf("\n");
	}

	printf("\n");
}

void dumpRegs()
{
	printf("i:\t%hx\n", r_i);
	printf("v0:\t%hhx\tv1:\t%hhx\tv2:\t%hhx\tv3:\t%hhx\t\n", r_v0, r_v1, r_v2, r_v3);
	printf("v4:\t%hhx\tv5:\t%hhx\tv6:\t%hhx\tv7:\t%hhx\t\n", r_v4, r_v5, r_v6, r_v7);
	printf("v8:\t%hhx\tv9:\t%hhx\tva:\t%hhx\tvb:\t%hhx\t\n", r_v8, r_v9, r_va, r_vb);
	printf("vc:\t%hhx\tvd:\t%hhx\tve:\t%hhx\tvf:\t%hhx\t\n", r_vc, r_vd, r_ve, r_vf);

	printf("pc:\t%hx\tsp:\t%hhx\n", r_pc, r_sp);
	printf("dt:\t%hhx\tst:\t%hhx\t", r_dt, r_st);

	printf("\n");
}

void dumpKeyState()
{
	printw("1 2 3 C: %d %d %d %d\n", key1, key2, key3, keyC);
	printw("4 5 6 D: %d %d %d %d\n", key4, key5, key6, keyD);
	printw("7 8 9 E: %d %d %d %d\n", key7, key8, key9, keyE);
	printw("A 0 B F: %d %d %d %d\n", keyA, key0, keyB, keyF);
}

bool xorToPoint(int x, int y, bool val)
{
	// wrap around
	x = x % 64;
	y = y % 32;

	int screenOffset = x + (64 * y);
	bool* screenPtr = &screen[screenOffset];

	bool prevScreenBool = *screenPtr;
	bool newScreenBool = val ^ prevScreenBool;

	*screenPtr = newScreenBool;

	// if the prev point became unset, return true
	return prevScreenBool & val;
}

bool kbhit(WINDOW* win)
{
	int ch = wgetch(win);
	if (ch != ERR)
	{
		ungetch(ch);
		return true;
	}
	else
	{
		return 0;
	}
}

// ensure program counter register is a valid address
bool validPC(unsigned short nnn)
{
	if (nnn > 0xffe)
		return false; // outside 4k memory
	if (nnn & 1)
		return false; // misalignment
	return true;
}

unsigned char* getVxReg(unsigned char x)
{
	switch (x)
	{
		case 0x0:
			return &r_v0;
		case 0x1:
			return &r_v1;
		case 0x2:
			return &r_v2;
		case 0x3:
			return &r_v3;
		case 0x4:
			return &r_v4;
		case 0x5:
			return &r_v5;
		case 0x6:
			return &r_v6;
		case 0x7:
			return &r_v7;
		case 0x8:
			return &r_v8;
		case 0x9:
			return &r_v9;
		case 0xa:
			return &r_va;
		case 0xb:
			return &r_vb;
		case 0xc:
			return &r_vc;
		case 0xd:
			return &r_vd;
		case 0xe:
			return &r_ve;
		case 0xf:
			return &r_vf;
		default:
			printf("Bad input to getVxReg: %x", x);
	}
}
