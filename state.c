#include <stdio.h>
#include <stdlib.h>
#include "state.h"

void initializeState()
{
	char *mem = malloc(4096);
	short *stack = malloc(16);

	short r_i = 0;
	char r_v0 = 0;
	char r_v1 = 0;
	char r_v2 = 0;
	char r_v3 = 0;
	char r_v4 = 0;
	char r_v5 = 0;
	char r_v6 = 0;
	char r_v7 = 0;
	char r_v8 = 0;
	char r_v9 = 0;
	char r_va = 0;
	char r_vb = 0;
	char r_vc = 0;
	char r_vd = 0;
	char r_ve = 0;
	char r_vf = 0;
	char r_pc = 0;
	char r_sp = 0;
	char r_dt = 0;
	char r_st = 0;
	
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
