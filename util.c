#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 
#include <ncurses.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
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

void dumpRegs(WINDOW* win)
{
	werase(win);
	wprintw(win, "i: %04hx\n", r_i);
	wprintw(win, "v0: %02hhx\tv1: %02hhx\tv2: %02hhx\tv3: %02hhx\n", r_v0, r_v1, r_v2, r_v3);
	wprintw(win, "v4: %02hhx\tv5: %02hhx\tv6: %02hhx\tv7: %02hhx\n", r_v4, r_v5, r_v6, r_v7);
	wprintw(win, "v8: %02hhx\tv9: %02hhx\tva: %02hhx\tvb: %02hhx\n", r_v8, r_v9, r_va, r_vb);
	wprintw(win, "vc: %02hhx\tvd: %02hhx\tve: %02hhx\tvf: %02hhx\n", r_vc, r_vd, r_ve, r_vf);

	wprintw(win, "pc: %04hx\tsp: %02hhx\n", r_pc, r_sp);
	wprintw(win, "dt: %02hhx\tst: %02hhx\n", r_dt, r_st);

	wrefresh(win);
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

bool kbhit()
{
	//wprintw(messageWin, "entering kbhit\n");
	//wrefresh(messageWin);

	// thanks kanwar
	// https://stackoverflow.com/a/13129698/5924962
	struct timeval tv;
	fd_set rdfs;

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	FD_ZERO(&rdfs);
	FD_SET(STDIN_FILENO, &rdfs);

	select(STDIN_FILENO + 1, &rdfs, NULL, NULL, &tv);
	int isset = FD_ISSET(STDIN_FILENO, &rdfs);

	//wprintw(messageWin, "leaving kbhit\n");
	//wrefresh(messageWin);

	return isset;
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

useconds_t calculateDelay(unsigned int hz)
{
	return 1000000 / hz;
}

void exitProgram(int returnval, char *message)
{
	delwin(drawWin);
	delwin(messageWin);
	delwin(registerWin);
	endwin();
	printf("%s\n", message);
	exit(returnval);
}
