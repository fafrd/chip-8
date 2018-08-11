#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "state.h"

unsigned char *mem;
unsigned short *stack;

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
	
void initializeState()
{
	mem = malloc(4096);
	stack = malloc(STACK_SIZE);

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
