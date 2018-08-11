#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>
#include "instructions.h"
#include "state.h"
#include "util.h"

#define QUIRK_SHIFT_RESULT_IN_VY false

// Store number NN in register VX
void i_6xnn(unsigned char x, unsigned char nn)
{
	unsigned char *vx = getVxReg(x);
	*vx = nn;
}

// Store the value of register VY in register VX
void i_8xy0(unsigned char x, unsigned char y)
{
	unsigned char *vx = getVxReg(x);
	unsigned char *vy = getVxReg(y);
	*vx = *vy;
}

// Add the value of register VY to register VX
// Set VF to 01 if a carry occurs
// Set VF to 00 if a carry does not occur
void i_8xy4(unsigned char x, unsigned char y)
{
	unsigned char *vx = getVxReg(x);
	unsigned char *vy = getVxReg(y);

	if (*vx + *vy > 0xff)
		r_vf = 0x01;
	else
		r_vf = 0x00;

	*vx = *vx + *vy;
}

// Subtract the value of register VY from register VX
// Set VF to 00 if a borrow occurs
// Set VF to 01 if a borrow does not occur
void i_8xy5(unsigned char x, unsigned char y)
{
	unsigned char *vx = getVxReg(x);
	unsigned char *vy = getVxReg(y);

	//printf("%x < %x = %d\n", *vx, *vy, *vx > *vy);

	if (*vx < *vy)
		r_vf = 0x01;
	else
		r_vf = 0x00;

	*vx = *vx - *vy;
}

// Set register VX to the value of VY minus VX
// Set VF to 00 if a borrow occurs
// Set VF to 01 if a borrow does not occur
void i_8xy7(unsigned char x, unsigned char y)
{
	unsigned char *vx = getVxReg(x);
	unsigned char *vy = getVxReg(y);

	//printf("%x > %x = %d\n", *vx, *vy, *vx > *vy);

	if (*vx > *vy)
		r_vf = 0x01;
	else
		r_vf = 0x00;

	*vx = *vy - *vx;
}

// Set VX to VX AND VY
void i_8xy2(unsigned char x, unsigned char y)
{
	unsigned char *vx = getVxReg(x);
	unsigned char *vy = getVxReg(y);

	*vx &= *vy;
}

// Set VX to VX OR VY
void i_8xy1(unsigned char x, unsigned char y)
{
	unsigned char *vx = getVxReg(x);
	unsigned char *vy = getVxReg(y);

	*vx |= *vy;
}

// Set VX to VX XOR VY
void i_8xy3(unsigned char x, unsigned char y)
{
	unsigned char *vx = getVxReg(x);
	unsigned char *vy = getVxReg(y);

	*vx ^= *vy;
}

// This one is a bit ambiguous.
// interpretation 1:
//     vy = vx shifted right 1, then vf = lsb(vx) prior to shift
// interpretation 2:
//     vx = vx shifted right 1, then vf = lsb(vx) prior to shift
// Most implementations seem to use interpretation 2, where there is no effect on vy.
// Setting QUIRK_SHIFT_RESULT_IN_VY to TRUE will store the result in vy.
void i_8xy6(unsigned char x, unsigned char y)
{
	unsigned char *vx = getVxReg(x);
	unsigned char *vy = getVxReg(y);

	//printf("%x. vx & 1 = %x\n", *vx, 0x01 & *vx);
	r_vf = 0x01 & *vx;
#if QUIRK_SHIFT_RESULT_IN_VY
	*vy = *vx >> 1;
#else
	*vx = *vx >> 1;
#endif
}

// Store the value of register VY shifted left one bit in register VX
// Set register VF to the most significant bit prior to the shift
void i_8xye(unsigned char x, unsigned char y)
{
	unsigned char *vx = getVxReg(x);
	unsigned char *vy = getVxReg(y);

	if (0x80 & *vx)
		r_vf = 0x01;
	else
		r_vf = 0x00;
#if QUIRK_SHIFT_RESULT_IN_VY
	*vy = *vx << 1;
#else
	*vx = *vx << 1;
#endif
}

// Jump to address NNN
void i_1nnn(unsigned short nnn)
{
	if (!validPC(nnn - 2))
	{
		printf("unsafe PC destination requested: %hx. cannot continue.\n", nnn - 2);
		exit(1);
	}

	r_pc = nnn;
	r_pc -= 2; // account for the fact that loop() will increment pc by 2
}

// Jump to address NNN + V0
void i_bnnn(unsigned short nnn)
{
	if (!validPC(nnn + r_v0 - 2))
	{
		printf("unsafe PC destination requested: %hx. cannot continue.\n", nnn + r_v0 - 2);
		exit(1);
	}

	r_pc = nnn + r_v0;
	r_pc -= 2; // account for the fact that loop() will increment pc by 2
}

// Execute subroutine starting at address NNN
void i_2nnn(unsigned short nnn)
{
	//printf("r_sp: %x, stack[r_sp]: %x, nnn: %x\n", r_sp, stack[r_sp], nnn);

	// prevent stack overflow
	if (r_sp >= STACK_SIZE)
	{
		printf("reached maximum stack size of %u. cannot continue.\n", STACK_SIZE);
		exit(1);
	}

	// increment stack pointer, push current location to the stack
	r_sp++;
	stack[r_sp] = r_pc;

	// set pc to new location nnn
	i_1nnn(nnn);
}

// Return from a subroutine
void i_00ee()
{
	//printf("r_sp: %x, stack[r_sp]: %x\n", r_sp, stack[r_sp]);
	// pop the stack into pc, decrement stack pointer
	if (r_sp > 0)
	{
		r_pc = stack[r_sp];
		r_sp--;
	}
}

// Skip the following instruction if the value of register VX equals NN
void i_3xnn(unsigned char x, unsigned char nn)
{
	unsigned char *vx = getVxReg(x);

	if (*vx == nn)
		r_pc += 2;
}

// Skip the following instruction if the value of register VX is equal to the value of register VY
void i_5xy0(unsigned char x, unsigned char y)
{
	unsigned char *vx = getVxReg(x);
	unsigned char *vy = getVxReg(y);

	if (*vx == *vy)
		r_pc += 2;
}

// Skip the following instruction if the value of register VX is not equal to NN
void i_4xnn(unsigned char x, unsigned char nn)
{
	unsigned char *vx = getVxReg(x);

	if (*vx != nn)
		r_pc += 2;
}

// Skip the following instruction if the value of register VX is not equal to the value of register VY
void i_9xy0(unsigned char x, unsigned char y)
{
	unsigned char *vx = getVxReg(x);
	unsigned char *vy = getVxReg(y);

	if (*vx != *vy)
		r_pc += 2;
}

// Store memory address NNN in register I
void i_annn(unsigned short nnn)
{
	r_i = nnn;
}

// Add the value stored in register VX to register I
void i_fx1e(unsigned char x)
{
	unsigned char *vx = getVxReg(x);
	r_i += *vx;
}

// Store the values of registers V0 to VX inclusive in memory starting at address I
// I is set to I + X + 1 after operation
void i_fx55(unsigned char x)
{
	// start with highest- execution will fall through
	// to remaining values
	// printf("i is %hx, x is %hhx\n", r_i, x);
	switch (x)
	{
		case 0xf:
			mem[r_i + 0xf] = *getVxReg(0xf);
		case 0xe:
			mem[r_i + 0xe] = *getVxReg(0xe);
		case 0xd:
			mem[r_i + 0xd] = *getVxReg(0xd);
		case 0xc:
			mem[r_i + 0xc] = *getVxReg(0xc);
		case 0xb:
			mem[r_i + 0xb] = *getVxReg(0xb);
		case 0xa:
			mem[r_i + 0xa] = *getVxReg(0xa);
		case 0x9:
			mem[r_i + 0x9] = *getVxReg(0x9);
		case 0x8:
			mem[r_i + 0x8] = *getVxReg(0x8);
		case 0x7:
			mem[r_i + 0x7] = *getVxReg(0x7);
		case 0x6:
			mem[r_i + 0x6] = *getVxReg(0x6);
		case 0x5:
			mem[r_i + 0x5] = *getVxReg(0x5);
		case 0x4:
			mem[r_i + 0x4] = *getVxReg(0x4);
		case 0x3:
			mem[r_i + 0x3] = *getVxReg(0x3);
		case 0x2:
			mem[r_i + 0x2] = *getVxReg(0x2);
		case 0x1:
			mem[r_i + 0x1] = *getVxReg(0x1);
		case 0x0:
			mem[r_i + 0x0] = *getVxReg(0x0);
	}

	unsigned char *vx = getVxReg(x);
	r_i += *vx + 1;
}

// Fill registers V0 to VX inclusive with the values stored in memory starting at address I
// I is set to I + X + 1 after operation
void i_fx65(unsigned char x)
{
	// opposite of fx55. fill registers with memory
	switch (x)
	{
		case 0xf:
			*getVxReg(0xf) = mem[r_i + 0xf];
		case 0xe:
			*getVxReg(0xe) = mem[r_i + 0xe];
		case 0xd:
			*getVxReg(0xd) = mem[r_i + 0xd];
		case 0xc:
			*getVxReg(0xc) = mem[r_i + 0xc];
		case 0xb:
			*getVxReg(0xb) = mem[r_i + 0xb];
		case 0xa:
			*getVxReg(0xa) = mem[r_i + 0xa];
		case 0x9:
			*getVxReg(0x9) = mem[r_i + 0x9];
		case 0x8:
			*getVxReg(0x8) = mem[r_i + 0x8];
		case 0x7:
			*getVxReg(0x7) = mem[r_i + 0x7];
		case 0x6:
			*getVxReg(0x6) = mem[r_i + 0x6];
		case 0x5:
			*getVxReg(0x5) = mem[r_i + 0x5];
		case 0x4:
			*getVxReg(0x4) = mem[r_i + 0x4];
		case 0x3:
			*getVxReg(0x3) = mem[r_i + 0x3];
		case 0x2:
			*getVxReg(0x2) = mem[r_i + 0x2];
		case 0x1:
			*getVxReg(0x1) = mem[r_i + 0x1];
		case 0x0:
			*getVxReg(0x0) = mem[r_i + 0x0];
	}

	unsigned char *vx = getVxReg(x);
	r_i += *vx + 1;
}

// Set the delay timer to the value of register VX
void i_fx15(unsigned char x)
{
	unsigned char *vx = getVxReg(x);
	r_dt = *vx;
}

// Store the current value of the delay timer in register VX
void i_fx07(unsigned char x)
{
	unsigned char *vx = getVxReg(x);
	*vx = r_dt;
}

// Set the sound timer to the value of register VX
void i_fx18(unsigned char x)
{
	unsigned char *vx = getVxReg(x);
	r_st = *vx;
}

// Set VX to a random number with a mask of NN
void i_cxnn(unsigned char x, unsigned char nn)
{
	unsigned char *vx = getVxReg(x);
	*vx = rand() % 0xff;
	*vx &= nn;
}

// Set I to the memory address of the sprite data corresponding to the hexadecimal digit stored in register VX
void i_fx29(unsigned char x)
{
	unsigned char *vx = getVxReg(x);
	switch (*vx)
	{
		case 0x0:
			r_i = 0x00;
			break;
		case 0x1:
			r_i = 0x0a;
			break;
		case 0x2:
			r_i = 0x14;
			break;
		case 0x3:
			r_i = 0x1e;
			break;
		case 0x4:
			r_i = 0x28;
			break;
		case 0x5:
			r_i = 0x32;
			break;
		case 0x6:
			r_i = 0x3c;
			break;
		case 0x7:
			r_i = 0x46;
			break;
		case 0x8:
			r_i = 0x50;
			break;
		case 0x9:
			r_i = 0x5a;
			break;
		case 0xa:
			r_i = 0x64;
			break;
		case 0xb:
			r_i = 0x6e;
			break;
		case 0xc:
			r_i = 0x78;
			break;
		case 0xd:
			r_i = 0x82;
			break;
		case 0xe:
			r_i = 0x8c;
			break;
		case 0xf:
			r_i = 0x96;
			break;
	}
}

// Store the binary-coded decimal equivalent of the value stored in register VX at addresses I, I+1, and I+2
void i_fx33(unsigned char x)
{
	unsigned char *vx = getVxReg(x);
	//printf("x: %x\n", x);
	unsigned char ones = *vx % 10;
	unsigned char tens = (*vx / 10) % 10;
	unsigned char hundreds = (*vx / 100) % 10;

	//printf("r_i: %x, ones: %x, tens: %x, hundreds: %x\n", r_i, ones, tens, hundreds);

	mem[r_i] = hundreds;
	mem[r_i + 1] = tens;
	mem[r_i + 2] = ones;
}

// Wait for a keypress and store the result in register VX
void i_fx0a(unsigned char x)
{
	unsigned char *vx = getVxReg(x);

	*vx = mapKey(getch());
}

// Skip the following instruction if the key corresponding to the hex value currently stored in register VX is pressed
void i_ex9e(unsigned char x)
{
	unsigned char *vx = getVxReg(x);

	if (getStateForKey(*vx))
		r_pc += 2;
}

// Skip the following instruction if the key corresponding to the hex value currently stored in register VX is not pressed
void i_exa1(unsigned char x)
{
	unsigned char *vx = getVxReg(x);

	if (!getStateForKey(*vx))
		r_pc += 2;
}


