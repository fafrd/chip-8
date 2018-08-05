#include <stdio.h>
#include "state.h"

// prints contents of mem to screen for debugging
void dumpMem()
{
	// TODO

	printf("\n");
}

void dumpRegs()
{
	printf("i:\t%x\n", r_i);
	printf("v0:\t%x\tv1:\t%x\tv2:\t%x\tv3:\t%x\t\n", r_v0, r_v1, r_v2, r_v3);
	printf("v4:\t%x\tv5:\t%x\tv6:\t%x\tv7:\t%x\t\n", r_v4, r_v5, r_v6, r_v7);
	printf("v8:\t%x\tv9:\t%x\tva:\t%x\tvb:\t%x\t\n", r_v8, r_v9, r_va, r_vb);
	printf("vc:\t%x\tvd:\t%x\tve:\t%x\tvf:\t%x\t\n", r_vc, r_vd, r_ve, r_vf);

	printf("pc:\t%x\tsp:\t%x\n", r_pc, r_sp);
	printf("dt:\t%x\tst:\t%x\t", r_dt, r_st);

	printf("\n");
}

char* getVxReg(char x)
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
