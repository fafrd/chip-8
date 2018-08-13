#include <stdio.h>
#include <stdlib.h>
#include "disassembler.h"
#include "state.h"

void readFile(char *filename, unsigned char **buffer, size_t *length)
{
	FILE *f = fopen(filename, "r");

	fseek(f, 0, SEEK_END);
	*length = (size_t)ftell(f);
	fseek(f, 0, SEEK_SET);

	*buffer = malloc(*length);
	fread(*buffer, *length, 1, f);
	fclose(f);
}

void disassemble(size_t length)
{
	// program starts at 0x200
	r_pc = 0x200;
	unsigned char current_upper, current_lower;
	unsigned short current;

	printf(" pc | raw  | instruction\n");
	printf("----------------------\n");

	while (1)
	{
		// set current instruction...
		// instructions are 2 bytes, get upper+lower byte, combine
		current_upper = mem[r_pc];
		current_lower = mem[r_pc+1];
		current = (((short)current_upper) << 8) | current_lower;

		printf("%03hx | %04hx | ", r_pc, current);

		// switch on first nibble (4 bits)
		switch (current_upper >> 4)
		{
			case 0x0:
			{
				if (current == 0x00e0)
				{
					// 00e0 - clear screen
					printf("CLS");
				}
				else if (current == 0x00ee)
				{
					// 00ee - return from subroutine
					printf("RET");
				}

				break;
			}
			case 0x1:
			{
				// jumpDest is bottom 12 bits
				unsigned short jumpDest = current & 0x0fff;
				// 1nnn - set pc to jumpDest
				printf("JP %03hx", jumpDest);
				break;
			}
			case 0x2:
			{
				// callDest is bottom 12 bits
				unsigned short callDest = current & 0x0fff;
				// 2nnn - set pc to callDest and increment sp
				printf("CALL %03hx", callDest);
				break;
			}
			case 0x3:
			{
				// target register is second nibble
				unsigned char targetReg = current_upper & 0x0f;
				// compare register with the last byte
				unsigned char byteToCompare = current_lower;
				// 3xnn - if targetReg == byteToCompare, increment pc by 2
				printf("SE V%hhX, %02hhx", targetReg, byteToCompare);
				break;
			}
			case 0x4:
			{
				// target register is second nibble
				unsigned char targetReg = current_upper & 0x0f;
				// compare register with the last byte
				unsigned char byteToCompare = current_lower;
				// 4xnn - if targetReg != byteToCompare, increment pc by 2
				printf("SNE V%hhX, %02hhx", targetReg, byteToCompare);
				break;
			}
			case 0x5:
			{
				// target register x is second nibble
				unsigned char targetRegX = current_upper & 0x0f;
				// target register y is third nibble
				unsigned char targetRegY = current_lower >> 4;
				// 5xy0 - if targetRegX == targetRegY, increment pc by 2
				printf("SE V%hhX, V%hhX", targetRegX, targetRegY);
				break;
			}
			case 0x6:
			{
				// target register is second nibble
				unsigned char targetReg = current_upper & 0x0f;
				// 6xnn - load current_lower into targetReg
				printf("LD V%hhX, %hhx", targetReg, current_lower);
				break;
			}
			case 0x7:
			{
				// target register is second nibble
				unsigned char targetReg = current_upper & 0x0f;
				// 7xnn - targetReg = targetReg + current_lower
				printf("ADD V%hhX, %02hhx", targetReg, current_lower);
				break;
			}
			case 0x8:
			{
				// target register x is second nibble
				unsigned char targetRegX = current_upper & 0x0f;
				// target register y is third nibble
				unsigned char targetRegY = current_lower >> 4;

				// switch on last nibble
				switch (current_lower & 0x0f)
				{
					case 0x0:
					{
						// 8xy0 - targetRegX = targetRegY
						printf("LD V%hhX, V%hhX", targetRegX, targetRegY);
						break;
					}
					case 0x1:
					{
						// 8xy1 - targetRegX = targetRegX | targetRegY
						printf("OR V%hhX, V%hhX", targetRegX, targetRegY);
						break;
					}
					case 0x2:
					{
						// 8xy2 - targetRegX = targetRegX & targetRegY
						printf("AND V%hhX, V%hhX", targetRegX, targetRegY);
						break;
					}
					case 0x3:
					{
						// 8xy3 - targetRegX = targetRegX ^ targetRegY
						printf("XOR V%hhX, V%hhX", targetRegX, targetRegY);
						break;
					}
					case 0x4:
					{
						// 8xy4 - targetRegX = targetRegX + targetRegY
						// if the result overflows, set register VF to 1,
						// and keep lower 8 bits in targetRegX.
						// else, set register VF to 0
						printf("ADD V%hhX, V%hhX", targetRegX, targetRegY);
						break;
					}
					case 0x5:
					{
						// 8xy5 - targetRegX = targetRegX - targetRegY
						// if X < Y (that is, the result would be less than 0),
						// set register VF to 1.
						// else, set register VF to 0
						printf("SUB V%hhX, V%hhX", targetRegX, targetRegY);
						break;
					}
					case 0x6:
					{
						// 8xy6 - targetRegX = targetRegY >> 1
						// set register VF to least-significant (shifted away) bit of targetRegY
						printf("SHR V%hhX {, V%hhX}", targetRegX, targetRegY);
						break;
					}
					case 0x7:
					{
						// 8xy7- targetRegX = targetRegY - targetRegX
						// if X > Y (that is, the result would be less than 0),
						// set register VF to 1.
						// else, set register VF to 0
						printf("SUBN V%hhX, V%hhX", targetRegX, targetRegY);
						break;
					}
					case 0xe:
					{
						// 8xye - targetRegX = targetRegY << 1
						// set register VF to most-significant (shifted away) bit of targetRegY
						printf("SHL V%hhX {, V%hhX}", targetRegX, targetRegY);
						break;
					}
				}

				break;
			}
			case 0x9:
			{
				// target register x is second nibble
				unsigned char targetRegX = current_upper & 0x0f;
				// target register y is third nibble
				unsigned char targetRegY = current_lower >> 4;
				// 9xy0- if targetRegX != targetRegY, increment pc by 2
				printf("SNE V%hhX, V%hhX", targetRegX, targetRegY);
				break;
			}
			case 0xa:
			{
				// newValue is bottom 12 bits
				unsigned short newValue = current & 0x0fff;
				// annn - set register I to newValue
				printf("LD I, %03hx", newValue);
				break;
			}
			case 0xb:
			{
				// jumpDest is bottom 12 bits
				unsigned short jumpDest = current & 0x0fff;
				// bnnn - set pc to newValue + register V0
				printf("JP V0, %03hx", jumpDest);
				break;
			}
			case 0xc:
			{
				// target register x is second nibble
				unsigned char targetRegX = current_upper & 0x0f;
				// cxnn - generate a random byte, then mask it
				// targetRegisterX = current_lower & (random byte)
				printf("RND V%hhX", targetRegX);
				break;
			}
			case 0xd:
			{
				// target register x is second nibble
				unsigned char targetRegX = current_upper & 0x0f;
				// target register y is third nibble
				unsigned char targetRegY = current_lower >> 4;
				// n is last nibble
				unsigned char n = current_lower & 0X0f;
				// 0xyn - read n bytes from memory, starting at memory location stored in 
				// the I register, and draw them as sprites to screen at coordinates specified as
				// (x, y) from targetRegX and targetRegY.
				// sprites are XORed to the screen.
				// if this causes any pixels to be erased, set register VF to 1, else 0.
				// sprites positioned outside the display must wrap to the other side.
				printf("DRW V%hhX, V%hhX, %hhx", targetRegX, targetRegY, n);
				break;
			}
			case 0xe:
			{
				// target register x is second nibble
				unsigned char targetRegX = current_upper & 0x0f;

				if (current_lower == 0x9e)
				{
					// ex9e - if key corresponding to the value of targetRegX is
					// currently pressed, increment pc by 2
					printf("SKP V%hhX", targetRegX);
				}
				else if (current_lower == 0xa1)
				{
					// exa1 - if key corresponding to the value of targetRegX is
					// currently not pressed, increment pc by 2
					printf("SKNP V%hhX", targetRegX);
				}

				break;
			}
			case 0xf:
			{
				// target register x is second nibble
				unsigned char targetRegX = current_upper & 0x0f;
				
				switch (current_lower)
				{
					case 0x07:
					{
						// fx07 - targetRegX = register DT
						printf("LD V%hhX, DT");
						break;
					}
					case 0x0a:
					{
						// fx0a - halt execution until any key is pressed, then
						// store the value of that key in targetRegisterX
						printf("LD V%hhX, K");
						break;
					}
					case 0x15:
					{
						// fx15 - register DT = targetRegX
						printf("LD DT, V%hhX", targetRegX);
						break;
					}
					case 0x18:
					{
						// fx18 - register ST = targetRegX
						printf("LD ST, V%hhX", targetRegX);
						break;
					}
					case 0x1e:
					{
						// fx1e - register i = register i + targetRegX
						printf("ADD I, V%hhX", targetRegX);
						break;
					}
					case 0x29:
					{
						// fx29 - register i = location of sprite specified by targetRegX
						// (not fully sure what this means yet)
						printf("LD F, V%hhX", targetRegX);
						break;
					}
					case 0x33:
					{
						// fx33 - store binary coded decimal representation of
						// targetRegX in memory locations i, i+1, i+2
						printf("LD B, V%hhX", targetRegX);
						break;
					}
					case 0x55:
					{
						// fx55 - store registers v0 through targetRegX in memory
						// starting at location i
						printf("LD [I], V%hhX", targetRegX);
						break;
					}
					case 0x65:
					{
						// fx65 - read registers v0 through targetRegX from memory
						// starting at location i
						printf("LD V%hhX, [I]", targetRegX);
						break;
					}
				}

				break;
			}
		}

		printf("\n");

		// increment pc by 2 bytes
		r_pc += 2;
		
		if (r_pc - 0x200 >= length)
			return;
	}
}

/*
void printBytes(char *buffer, size_t length)
{
	for (int i = 0; i < length; i++)
	{
		if (i && i % 8 == 0)
			printf("\n");
		if (!(i % 8 == 0) && (i % 2 == 0))
			printf("\t");
		//printf("%hhx ", buffer[i]);
		char current_upper = buffer[i++];
		char current_lower = buffer[i];
		short current = (((short)current_upper) << 8) | (0x00ff & current_lower);
		printf("%hhx %hhx: %hx", current_upper, current_lower, current);
	}

	printf("%c", '\n');
}
*/
