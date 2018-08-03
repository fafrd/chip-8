#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disassembler.h"
#include "state.h"

// copies buffer to mem starting at mem[0x200]
// returns 0 on success
int loadRomToMemory(char **buffer, size_t length)
{
	if (length > 0xc8f)
		return 1; // rom too big
	memcpy((mem + 0x200), *buffer, length);
	return 0;
}

// main instruction processing loop
void loop()
{
	// program starts at 0x200
	r_pc = 0x200;
	char current_upper, current_lower;
	short current;

	while (1)
	{
		// set current instruction...
		// instructions are 2 bytes, get upper+lower byte, combine
		current_upper = mem[r_pc];
		current_lower = mem[r_pc+1];
		current = (((short)current_upper) << 8) | current_lower;

		// switch on first nibble (4 bits)
		switch (current_upper >> 4)
		{
			case 0x0:
			{
				if (current == 0x00e0)
					// TODO CLS - clear screen
				else if (current == 0x00ee)
					// TODO RET - return from subroutine
			}
			case 0x1:
			{
				// jumpDest is bottom 12 bits
				short jumpDest = current & 0x0fff;
				// TODO JP - set pc to jumpDest
			}
			case 0x2:
			{
				// callDest is bottom 12 bits
				short callDest = current & 0x0fff;
				// TODO CALL - set pc to callDest and increment sp
			}
			case 0x3:
			{
				// target register is second nibble
				char targetReg = current_upper & 0x0f;
				// compare register with the last byte
				char byteToCompare = current_lower;

				// TODO SE - if targetReg == byteToCompare, increment pc by 2
			}
			case 0x4:
			{
				// target register is second nibble
				char targetReg = current_upper & 0x0f;
				// compare register with the last byte
				char byteToCompare = current_lower;

				// TODO SNE - if targetReg != byteToCompare, increment pc by 2
			}
			case 0x5:
			{
				// target register x is second nibble
				char targetRegX = current_upper & 0x0f;
				// target register y is third nibble
				char targetRegY = current_lower >> 4;

				// TODO SE - if targetRegX == targetRegY, increment pc by 2
			}
			case 0x6:
			{
				// target register is second nibble
				char targetReg = current_upper & 0x0f;

				// TODO LD - load current_lower into targetReg
			}
			case 0x7:
			{
				// target register is second nibble
				char targetReg = current_upper & 0x0f;

				// TODO ADD - targetReg = targetReg + current_lower
			}
			case 0x8:
			{
				// target register x is second nibble
				char targetRegX = current_upper & 0x0f;
				// target register y is third nibble
				char targetRegY = current_lower >> 4;

				// switch on last nibble
				switch (current_lower & 0x0f)
				{
					case 0x0:
					{
						// TODO LD - targetRegX = targetRegY
					}
					case 0x1:
					{
						// TODO OR - targetRegX = targetRegX | targetRegY
					}
					case 0x2:
					{
						// TODO AND - targetRegX = targetRegX & targetRegY

					}
					case 0x3:
					{
						// TODO XOR - targetRegX = targetRegX ^ targetRegY
					}
					case 0x4:
					{
						// TODO ADD - targetRegX = targetRegX + targetRegY
						// if the result overflows, set register VF to 1,
						// and keep lower 8 bits in targetRegX.
						// else, set register VF to 0
					}
					case 0x5:
					{
						// TODO SUB - targetRegX = targetRegX - targetRegY
						// if X < Y (that is, the result would be less than 0),
						// set register VF to 1.
						// else, set register VF to 0
					}
					case 0x6:
					{
						// TODO SHR - targetRegX = targetRegY >> 1
						// set register VF to least-significant (shifted away) bit of targetRegY
					}
					case 0x7:
					{
						// TODO SUB - targetRegX = targetRegY - targetRegX
						// if X > Y (that is, the result would be less than 0),
						// set register VF to 1.
						// else, set register VF to 0
					}
					case 0xe:
					{
						// TODO SHL - targetRegX = targetRegY << 1
						// set register VF to most-significant (shifted away) bit of targetRegY
					}
				}
			}
			case 0x9:
			{
				// target register x is second nibble
				char targetRegX = current_upper & 0x0f;
				// target register y is third nibble
				char targetRegY = current_lower >> 4;

				// TODO SNE - if targetRegX != targetRegY, increment pc by 2
			}
			case 0xa:
			{
				// newValue is bottom 12 bits
				short newValue = current & 0x0fff;

				// TODO LD - set register I to newValue
			}
			case 0xb:
			{
				// jumpDest is bottom 12 bits
				short jumpDest = current & 0x0fff;

				// TODO JP - set pc to newValue + register V0
			}
			case 0xc:
			{
				// target register x is second nibble
				char targetRegX = current_upper & 0x0f;

				// TODO RND - generate a random byte
				// targetRegisterX = current_lower & (random byte)
			}
			case 0xd:
			{
				// target register x is second nibble
				char targetRegX = current_upper & 0x0f;
				// target register y is third nibble
				char targetRegY = current_lower >> 4;
				// n is last nibble
				char n = current_lower & 0X0f;

				// TODO DRW - read n bytes from memory, starting at memory location stored in 
				// the I register, and draw them as sprites to screen at coordinates specified as
				// (x, y) from targetRegX and targetRegY.
				// sprites are XORed to the screen.
				// if this causes any pixels to be erased, set register VF to 1, else 0.
				// sprites positioned outside the display must wrap to the other side.
			}
			case 0xe:
			{
				// target register x is second nibble
				char targetRegX = current_upper & 0x0f;

				if (current_lower == 0x9e)
				{
					// TODO SKP - if key corresponding to the value of targetRegX is
					// currently pressed, increment pc by 2
				}
				else if (current_lower == 0xa1)
				{
					// TODO SKNP - if key corresponding to the value of targetRegX is
					// currently not pressed, increment pc by 2
				}
			}
			case 0xf:
			{
				// target register x is second nibble
				char targetRegX = current_upper & 0x0f;
				
				switch (current_lower)
				{
					case 0x07:
					{
						// TODO LD - targetRegX = register DT
					}
					case 0x0a:
					{
						// TODO LD - halt execution until any key is pressed, then
						// store the value of that key in targetRegisterX
					}
					case 0x15:
					{
						// TODO LD - register DT = targetRegX
					}
					case 0x18:
					{
						// TODO LD - register ST = targetRegX
					}
					case 0x1e:
					{
						// TODO ADD - register i = register i + targetRegX
					}
					case 0x29:
					{
						// TODO LD - 
					}
					case 0x33:
					{
					}
					case 0x55:
					{
					}
					case 0x65:
					{
					}
				}
			}
		}

		// increment pc by 2 bytes
		r_pc += 2;
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("%s", "usage: emulator file.rom\n");
		return -1;
	}

	// Initialize registers, memory
	initializeState();

	// read romfile into buffer
	char *buffer;
	size_t length;
	readFile(argv[1], &buffer, &length);

	int loadSuccess = loadRomToMemory(&buffer, length);
	if (loadSuccess != 0)
	{
		printf("%s", "error: ROM size greater than 0xc8f bytes\n");
		return 1;
	}

	// start main program processing loop
	loop();

	//printBytes(buffer, length);

	free(buffer);
	return 0;
}
