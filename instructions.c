#include <stdio.h>
#include <stdlib.h>
#include "instructions.h"
#include "state.h"
#include "util.h"

// Store number NN in register VX
void i_6xnn(char x, char nn)
{
	char *vx = getVxReg(x);
	*vx = nn;
}

// Store the value of register VY in register VX
void i_8xy0(char x, char y)
{
	char *vx = getVxReg(x);
	char *vy = getVxReg(y);
	*vx = *vy;
}

// Add the value of register VY to register VX
// Set VF to 01 if a carry occurs
// Set VF to 00 if a carry does not occur
void i_8xy4(char x, char y)
{
	char *vx = getVxReg(x);
	char *vy = getVxReg(y);
	*vx = *vx + *vy;

	if (*vx + *vy > 0xff)
		r_vf = 0x01;
	else
		r_vf = 0x00;
}

// Subtract the value of register VY from register VX
// Set VF to 00 if a borrow occurs
// Set VF to 01 if a borrow does not occur
void i_8xy5(char x, char y)
{

}

// Set register VX to the value of VY minus VX
// Set VF to 00 if a borrow occurs
// Set VF to 01 if a borrow does not occur
void i_8xy7(char x, char y)
{

}
