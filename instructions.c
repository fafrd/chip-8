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
