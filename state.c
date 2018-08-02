#include <stdio.h>
#include <stdlib.h>
#include "state.h"

char *mem;
short *stack;

short r_i;
char r_v0;
char r_v1;
char r_v2;
char r_v3;
char r_v4;
char r_v5;
char r_v6;
char r_v7;
char r_v8;
char r_v9;
char r_va;
char r_vb;
char r_vc;
char r_vd;
char r_ve;
char r_vf;

short r_pc;
char r_sp;
char r_dt;
char r_st;
	
void initializeState()
{
	mem = malloc(4096);
	stack = malloc(16);

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
}

// prints contents of mem to screen for debugging
void printMem()
{
	// TODO
}
