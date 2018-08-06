#include <stdio.h>
#include <stdlib.h>
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

	printf("%x. vx & 1 = %x\n", *vx, 0x01 & *vx);
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

	// FIXME vf should be 1 or 0
	if (0x80 & *vx)
		r_vf = 0x01;
#if QUIRK_SHIFT_RESULT_IN_VY
	*vy = *vx << 1;
#else
	*vx = *vx << 1;
#endif
}

