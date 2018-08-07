#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
	printf("r_sp: %x, stack[r_sp]: %x, nnn: %x\n", r_sp, stack[r_sp], nnn);

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





