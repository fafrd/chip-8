#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>
#include "state.h"
#include "util.h"

unsigned char *mem;
unsigned short *stack;
bool *screen;

unsigned short r_i;
unsigned char r_v0;
unsigned char r_v1;
unsigned char r_v2;
unsigned char r_v3;
unsigned char r_v4;
unsigned char r_v5;
unsigned char r_v6;
unsigned char r_v7;
unsigned char r_v8;
unsigned char r_v9;
unsigned char r_va;
unsigned char r_vb;
unsigned char r_vc;
unsigned char r_vd;
unsigned char r_ve;
unsigned char r_vf;

unsigned short r_pc;
unsigned char r_sp;
unsigned char r_dt;
unsigned char r_st;

bool key0 = false;
bool key1 = false;
bool key2 = false;
bool key3 = false;
bool key4 = false;
bool key5 = false;
bool key6 = false;
bool key7 = false;
bool key8 = false;
bool key9 = false;
bool keyA = false;
bool keyB = false;
bool keyC = false;
bool keyD = false;
bool keyE = false;
bool keyF = false;

WINDOW* createDrawWindow()
{
	WINDOW* win;
	int height = 32;
	int width = 64;
	width *= 2; // characters are too narrow to be pixels
	win = newwin(height + 2, width + 2, (LINES - height) / 2, (COLS - width) / 2);
	box(win, 0, 0);
	wrefresh(win);
	return win;
}

void* updateTimers(void* arg)
{
	useconds_t sixtyHzDelay = 16667;
	while (1)
	{
		//printf("timer sleep...\n");
		usleep(sixtyHzDelay);
		if (r_dt > 0)
			r_dt--;
		if (r_st > 0)
			r_st--;
	}
}

unsigned char mapKey(int getchResult)
{
	//  1 2 3 C                                   1 2 3 4
	//  4 5 6 D    This is emulated with these    Q W E R
	//  7 8 9 E    keyboard keys -->              A S D F
	//  A 0 B F                                   Z X C V

	switch (getchResult)
	{
		case 49: // keyboard 1
			return 0x1;
		case 50: // keyboard 2
			return 0x2;
		case 51: // keyboard 3
			return 0x3;
		case 52: // keyboard 4
			return 0xC;
		case 113: // keyboard Q
			return 0x4;
		case 119: // keyboard W
			return 0x5;
		case 101: // keyboard E
			return 0x6;
		case 114: // keyboard R
			return 0xD;
		case 97: // keyboard A
			return 0x7;
		case 115: // keyboard S
			return 0x8;
		case 100: // keyboard D
			return 0x9;
		case 102: // keyboard F
			return 0xE;
		case 122: // keyboard Z
			return 0xA;
		case 120: // keyboard X
			return 0x0;
		case 99: // keyboard C
			return 0xB;
		case 118: // keyboard V
			return 0xF;
		default:
			return 0x0;
	}
}

void updateKeyState(unsigned char chipKey)
{
	switch (chipKey)
	{
		case 0x1:
			clearAllKeyStates();
			key1 = true;
			break;
		case 0x2:
			clearAllKeyStates();
			key2 = true;
			break;
		case 0x3:
			clearAllKeyStates();
			key3 = true;
			break;
		case 0x4:
			clearAllKeyStates();
			key4 = true;
			break;
		case 0x5:
			clearAllKeyStates();
			key5 = true;
			break;
		case 0x6:
			clearAllKeyStates();
			key6 = true;
			break;
		case 0x7:
			clearAllKeyStates();
			key7 = true;
			break;
		case 0x8:
			clearAllKeyStates();
			key8 = true;
			break;
		case 0x9:
			clearAllKeyStates();
			key9 = true;
			break;
		case 0x0:
			clearAllKeyStates();
			key0 = true;
			break;
		case 0xA:
			clearAllKeyStates();
			keyA = true;
			break;
		case 0xB:
			clearAllKeyStates();
			keyB = true;
			break;
		case 0xC:
			clearAllKeyStates();
			keyC = true;
			break;
		case 0xD:
			clearAllKeyStates();
			keyD = true;
			break;
		case 0xE:
			clearAllKeyStates();
			keyE = true;
			break;
		case 0xF:
			clearAllKeyStates();
			keyF = true;
			break;
	}
}

bool getStateForKey(unsigned char chipKey)
{
	switch (chipKey)
	{
		case 0x1:
			return key1;
		case 0x2:
			return key2;
		case 0x3:
			return key3;
		case 0x4:
			return key4;
		case 0x5:
			return key5;
		case 0x6:
			return key6;
		case 0x7:
			return key7;
		case 0x8:
			return key8;
		case 0x9:
			return key9;
		case 0x0:
			return key0;
		case 0xA:
			return keyA;
		case 0xB:
			return keyB;
		case 0xC:
			return keyC;
		case 0xD:
			return keyD;
		case 0xE:
			return keyE;
		case 0xF:
			return keyF;
	}

}

void clearAllKeyStates()
{
	key0 = false;
	key1 = false;
	key2 = false;
	key3 = false;
	key4 = false;
	key5 = false;
	key6 = false;
	key7 = false;
	key8 = false;
	key9 = false;
	keyA = false;
	keyB = false;
	keyC = false;
	keyD = false;
	keyE = false;
	keyF = false;
}

void clearScreen()
{
	memset(screen, 0, sizeof(bool) * 2048);
}

void initializeState()
{
	mem = calloc(4096, sizeof(unsigned char));
	stack = calloc(STACK_SIZE, sizeof(unsigned short));
	screen = calloc(2048, sizeof(bool));

	r_i = 0;
	r_v0 = 0;
	r_v1 = 0;
	r_v2 = 0;
	r_v3 = 0;
	r_v4 = 0;
	r_v5 = 0;
	r_v6 = 0;
	r_v7 = 0;
	r_v8 = 0;
	r_v9 = 0;
	r_va = 0;
	r_vb = 0;
	r_vc = 0;
	r_vd = 0;
	r_ve = 0;
	r_vf = 0;
	r_pc = 0;
	r_sp = 0;
	r_dt = 0;
	r_st = 0;
	
	r_pc = 0;
	r_sp = 0;
	r_dt = 0;
	r_st = 0;

	// create thread for delay/sound timers
	pthread_t timerThread;
	if (pthread_create(&timerThread, NULL, updateTimers, NULL))
	{
		fprintf(stderr, "Error creating timer thread\n");
	}

	if (pthread_detach(timerThread))
	{
		fprintf(stderr, "Error detaching timer thread\n");
	}

	// make rand() random
	srand(time(NULL));

	// load default sprites to memory
	char defaultSprites[80] = { 0xF0, 0x90, 0x90, 0x90, 0xF0, 0x20, 0x60, 0x20, 0x20, 0x70, 0xF0, 0x10, 0xF0, 0x80, 0xF0, 0xF0, 0x10, 0xF0, 0x10, 0xF0, 0x90, 0x90, 0xF0, 0x10, 0x10, 0xF0, 0x80, 0xF0, 0x10, 0xF0, 0xF0, 0x80, 0xF0, 0x90, 0xF0, 0xF0, 0x10, 0x20, 0x40, 0x40, 0xF0, 0x90, 0xF0, 0x90, 0xF0, 0xF0, 0x90, 0xF0, 0x10, 0xF0, 0xF0, 0x90, 0xF0, 0x90, 0x90, 0xE0, 0x90, 0xE0, 0x90, 0xE0, 0xF0, 0x80, 0x80, 0x80, 0xF0, 0xE0, 0x90, 0x90, 0x90, 0xE0, 0xF0, 0x80, 0xF0, 0x80, 0xF0, 0xF0, 0x80, 0xF0, 0x80, 0x80 };
	memcpy(mem, defaultSprites, 80);
}
