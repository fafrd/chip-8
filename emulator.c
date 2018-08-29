#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 
#include <unistd.h>
#include <ncurses.h>
#include <termios.h>
#include "disassembler.h"
#include "state.h"
#include "instructions.h"
#include "util.h"

// copies buffer to mem starting at mem[0x200]
// returns 0 on success
bool loadRomToMemory(unsigned char **buffer, size_t length)
{
	if (length > 0xc8f)
		return false; // rom too big
	memcpy((mem + 0x200), *buffer, length);
	return true;
}

void intro()
{
	mvwprintw(drawWin, 4, 6, "CHIP-8 INTERPRETER");
	mvwprintw(drawWin, 5, 10, "by kian");

	mvwprintw(drawWin,  7, 6, "controls:");
	mvwprintw(drawWin,  8, 6, "  esc            exit program");
	mvwprintw(drawWin,  9, 6, "  space          pause/resume emulation");
	mvwprintw(drawWin, 10, 6, "/ slash          step emulation");
	mvwprintw(drawWin, 11, 6, ", comma          reduce emulation speed");
	mvwprintw(drawWin, 12, 6, ". period         increase emulation speed");
	mvwprintw(drawWin, 13, 6, "; semicolon      toggle live printing of instructions");
	mvwprintw(drawWin, 14, 6, "' apostrophe     toggle live display of register");
	mvwprintw(drawWin, 15, 6, "[ left bracket   set shift result quirk");
	mvwprintw(drawWin, 16, 6, "] right bracket  set i register quirk");

	mvwprintw(drawWin, 28, 32, "Press any key to begin...");

	wrefresh(drawWin);
	getchar();
}

// main instruction processing loop
void loop(unsigned int hz)
{
	initscr(); // start curses mode

	cbreak(); // allow ctrl c, ctrl z to work
	noecho(); // don't echo what the user types
	curs_set(0); // cursor is invisible

	// create main window for CHIP-8 display
	createDrawWindow();
	nodelay(drawWin, TRUE); // sets getch() to be a non-blocking call

	// create secondary window for messages
	createMessageWindow();
	nodelay(messageWin, TRUE); // sets getch() to be a non-blocking call
	scrollok(messageWin, TRUE);

	// create window to print register states
	createRegisterWindow();
	nodelay(registerWin, TRUE); // sets getch() to be a non-blocking call
	scrollok(registerWin, FALSE);

	// change terminal mode, allowing raw keypresses to be read
	static struct termios oldt, newt;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	// hacky way to reset key state. see kbhit() call below
	int keyResetCounter = 0;
	const int counterMax = 50;

	// show splash screen
	intro();

	wprintw(messageWin, "Starting interpreter.\n");
	wrefresh(messageWin);

	// program starts at 0x200
	r_pc = 0x200;
	unsigned char current_upper, current_lower;
	unsigned short current;

	// set wait time between cpu cycles (default 300hz)
	if (hz < 1)
		hz = 300;
	useconds_t delayTime = calculateDelay(hz);

	// initialize variable containing emulation command requested
	COMMAND com = UNDEFINED;
	bool paused = false;
	bool step = false;
	bool printInstructions = false;
	bool printRegisters = true;

	while (1)
	{
		// draw previous loop output
		delwin(drawWin);
		createDrawWindow();

		// update register state
		if (printRegisters)
		{
			createRegisterWindow();
		}
		else if (registerWin != NULL)
		{
			wclear(registerWin);
			wrefresh(registerWin);
			registerWin = NULL;
		}

		// delay between each instruction
		usleep(delayTime);

		// update key pressed state
		com = UNDEFINED;
		if (kbhit())
		{
			int ch = getchar();
			unsigned char keyHit = mapKey(ch);
			//wprintw(messageWin, "key: %c, keyhit: %x\n", ch, keyHit);
			//wrefresh(messageWin);
			if (keyHit != 0xff)
				updateKeyState(keyHit);
			else
				com = parseCommandKey(ch);

			keyResetCounter = 0;
		}
		else
		{
			// this is a hacky way to deal with no-key-pressed event...
			// the way kbhit() works is that it returns when a key is pressed,
			// but after it is pressed the key press state is not true until
			// the key is sent AGAIN to the terminal. This effectively means that,
			// while a key is held down, we have to implement a delay, otherwise
			// the key is considered un-pressed until the terminal receives a
			// repeated key hit, as if typing aaaaaaaa by holding down the a key
			if (keyResetCounter >= counterMax)
			{
				clearAllKeyStates();
				keyResetCounter = 0;
			}
			else
			{
				keyResetCounter++;
			}
		}
commandParser:
		//wprintw(messageWin, "com: %d\n", com);
		//wrefresh(messageWin);
		switch (com)
		{
		
			case ESCAPE:
				exitProgram(0, "Emulation halted.");
				break;
			case PAUSE:
				if (paused)
				{
					wprintw(messageWin, "Emulation resumed.\n");
					wrefresh(messageWin);
					// if already paused, unpause
					paused = false;
					break;
				}
				else
				{
					wprintw(messageWin, "Emulation paused.\n");
					wrefresh(messageWin);
					paused = true;
					// wait for a new key. when something happens, reenter this statement
					com = parseCommandKey(getch());
					goto commandParser;
				}
				break;
			case STEP:
				wprintw(messageWin, "Step...\n");
				wrefresh(messageWin);
				paused = false;
				step = true;
				break;
			case SPEED_DOWN:
				if (hz > 10)
				{
					hz -= 10;
					delayTime = calculateDelay(hz);
					wprintw(messageWin, "CPU speed decreased from %dhz to %dhz\n", hz+10, hz);
					wrefresh(messageWin);
				}
				else if (hz > 1)
				{
					hz--;
					delayTime = calculateDelay(hz);
					wprintw(messageWin, "CPU speed decreased from %dhz to %dhz\n", hz+1, hz);
					wrefresh(messageWin);
				}
				break;
			case SPEED_UP:
				if (hz < 10)
				{
					hz++;
					delayTime = calculateDelay(hz);
					wprintw(messageWin, "CPU speed increased from %dhz to %dhz\n", hz-1, hz);
					wrefresh(messageWin);

				}
				else if (hz < 999990)
				{
					hz += 10;
					delayTime = calculateDelay(hz);
					wprintw(messageWin, "CPU speed increased from %dhz to %dhz\n", hz-10, hz);
					wrefresh(messageWin);
				}
				break;
			case PRINT_INSTRUCTIONS:
				if (!printInstructions)
				{
					printInstructions = true;
					wprintw(messageWin, "Instruction printing enabled.\n");
					wrefresh(messageWin);
				}
				else
				{
					printInstructions = false;
					wprintw(messageWin, "Instruction printing disabled.\n");
					wrefresh(messageWin);
				}
				break;
			case PRINT_REGISTERS:
				if (!printRegisters)
				{
					printRegisters = true;
					wprintw(messageWin, "Register display enabled.\n");
					wrefresh(messageWin);
				}
				else
				{
					printRegisters = false;
					wprintw(messageWin, "Register display disabled.\n");
					wrefresh(messageWin);
				}	
				break;
			case QUIRK_SHIFT:
				if (!QUIRK_SHIFT_RESULT_IN_VY)
				{
					QUIRK_SHIFT_RESULT_IN_VY = true;
					wprintw(messageWin, "Instructions 8xy6, 8xye will now place the result of the shift operation into register y.\n");
					wrefresh(messageWin);
				}
				else
				{
					QUIRK_SHIFT_RESULT_IN_VY = false;
					wprintw(messageWin, "Instructions 8xy6, 8xye will now place the result of the shift operation into register x.\n");
					wrefresh(messageWin);
				}
				break;
			case QUIRK_I:
				if (!QUIRK_SET_I_AFTER_LOAD_STORE)
				{
					QUIRK_SET_I_AFTER_LOAD_STORE = true;
					wprintw(messageWin, "Instructions fx55, fx65 will now set the I register to i + vx + 1.\n");
					wrefresh(messageWin);
				}
				else
				{
					QUIRK_SET_I_AFTER_LOAD_STORE = false;
					wprintw(messageWin, "Instructions fx55, fx65 will not affect the I register.\n");
					wrefresh(messageWin);
				}
				break;
		}

		if (paused)
		{
			com = parseCommandKey(getch()); // wait on a keypress
			goto commandParser; // we're paused, so bypass emulation loop, parse another command
		}

		if (step)
		{
			step = false;
			paused = true;
		}

		// set current instruction...
		// instructions are 2 bytes, get upper+lower byte, combine
		current_upper = mem[r_pc];
		current_lower = mem[r_pc+1];
		current = (((short)current_upper) << 8) | current_lower;

		if (printInstructions)
		{
			wprintw(messageWin, "pc: %03hx, instruction: %04hx\n", r_pc, current);
			wrefresh(messageWin);
		}

		// switch on first nibble (4 bits)
		switch (current_upper >> 4)
		{
			case 0x0:
			{
				// special instruction to exit for debugging (delete this later)
				if (current == 0x0033)
					return;

				if (current == 0x00e0)
				{
					// 00e0 - clear screen
					i_00e0();
				}
				else if (current == 0x00ee)
				{
					// 00ee - return from subroutine
					i_00ee();
				}

				break;
			}
			case 0x1:
			{
				// jumpDest is bottom 12 bits
				unsigned short jumpDest = current & 0x0fff;
				// 1nnn - set pc to jumpDest
				i_1nnn(jumpDest);

				break;
			}
			case 0x2:
			{
				// callDest is bottom 12 bits
				unsigned short callDest = current & 0x0fff;
				// 2nnn - set pc to callDest and increment sp
				i_2nnn(callDest);

				break;
			}
			case 0x3:
			{
				// target register is second nibble
				unsigned char targetReg = current_upper & 0x0f;
				// compare register with the last byte
				unsigned char byteToCompare = current_lower;

				// 3xnn - if targetReg == byteToCompare, increment pc by 2
				i_3xnn(targetReg, byteToCompare);

				break;
			}
			case 0x4:
			{
				// target register is second nibble
				unsigned char targetReg = current_upper & 0x0f;
				// compare register with the last byte
				unsigned char byteToCompare = current_lower;

				// 4xnn - if targetReg != byteToCompare, increment pc by 2
				i_4xnn(targetReg, byteToCompare);

				break;
			}
			case 0x5:
			{
				// target register x is second nibble
				unsigned char targetRegX = current_upper & 0x0f;
				// target register y is third nibble
				unsigned char targetRegY = current_lower >> 4;

				// 5xy0 - if targetRegX == targetRegY, increment pc by 2
				i_5xy0(targetRegX, targetRegY);

				break;
			}
			case 0x6:
			{
				// target register is second nibble
				unsigned char targetReg = current_upper & 0x0f;

				// 6xnn - load current_lower into targetReg
				i_6xnn(targetReg, current_lower);
				break;
			}
			case 0x7:
			{
				// target register is second nibble
				unsigned char targetReg = current_upper & 0x0f;

				// 7xnn - targetReg = targetReg + current_lower
				i_7xnn(targetReg, current_lower);
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
						i_8xy0(targetRegX, targetRegY);
						break;
					}
					case 0x1:
					{
						// 8xy1 - targetRegX = targetRegX | targetRegY
						i_8xy1(targetRegX, targetRegY);
						break;
					}
					case 0x2:
					{
						// 8xy2 - targetRegX = targetRegX & targetRegY
						i_8xy2(targetRegX, targetRegY);
						break;
					}
					case 0x3:
					{
						// 8xy3 - targetRegX = targetRegX ^ targetRegY
						i_8xy3(targetRegX, targetRegY);
						break;
					}
					case 0x4:
					{
						// 8xy4 - targetRegX = targetRegX + targetRegY
						// if the result overflows, set register VF to 1,
						// and keep lower 8 bits in targetRegX.
						// else, set register VF to 0
						i_8xy4(targetRegX, targetRegY);
						break;
					}
					case 0x5:
					{
						// 8xy5 - targetRegX = targetRegX - targetRegY
						// if X < Y (that is, the result would be less than 0),
						// set register VF to 1.
						// else, set register VF to 0
						i_8xy5(targetRegX, targetRegY);
						break;
					}
					case 0x6:
					{
						// 8xy6 - targetRegX = targetRegY >> 1
						// set register VF to least-significant (shifted away) bit of targetRegY
						i_8xy6(targetRegX, targetRegY);
						break;
					}
					case 0x7:
					{
						// 8xy7- targetRegX = targetRegY - targetRegX
						// if X > Y (that is, the result would be less than 0),
						// set register VF to 1.
						// else, set register VF to 0
						i_8xy7(targetRegX, targetRegY);
						break;
					}
					case 0xe:
					{
						// 8xye - targetRegX = targetRegY << 1
						// set register VF to most-significant (shifted away) bit of targetRegY
						i_8xye(targetRegX, targetRegY);
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
				i_9xy0(targetRegX, targetRegY);

				break;
			}
			case 0xa:
			{
				// newValue is bottom 12 bits
				unsigned short newValue = current & 0x0fff;

				// annn - set register I to newValue
				i_annn(newValue);

				break;
			}
			case 0xb:
			{
				// jumpDest is bottom 12 bits
				unsigned short jumpDest = current & 0x0fff;

				// bnnn - set pc to newValue + register V0
				i_bnnn(jumpDest);

				break;
			}
			case 0xc:
			{
				// target register x is second nibble
				unsigned char targetRegX = current_upper & 0x0f;

				// cxnn - generate a random byte, then mask it
				// targetRegisterX = current_lower & (random byte)
				i_cxnn(targetRegX, current_lower);

				break;
			}
			case 0xd:
			{
				// target register x is second nibble
				unsigned char targetRegX = current_upper & 0x0f;
				// target register y is third nibble
				unsigned char targetRegY = current_lower >> 4;
				// n is last nibble
				unsigned char n = current_lower & 0x0f;

				// dxyn - read n bytes from memory, starting at memory location stored in 
				// the I register, and draw them as sprites to screen at coordinates specified as
				// (x, y) from targetRegX and targetRegY.
				// sprites are XORed to the screen.
				// if this causes any pixels to be erased, set register VF to 1, else 0.
				// sprites positioned outside the display must wrap to the other side.
				i_dxyn(targetRegX, targetRegY, n);

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
					i_ex9e(targetRegX);
				}
				else if (current_lower == 0xa1)
				{
					// exa1 - if key corresponding to the value of targetRegX is
					// currently not pressed, increment pc by 2
					i_exa1(targetRegX);
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
						i_fx07(targetRegX);
						break;
					}
					case 0x0a:
					{
						// fx0a - halt execution until any key is pressed, then
						// store the value of that key in targetRegisterX
						i_fx0a(targetRegX);
						break;
					}
					case 0x15:
					{
						// fx15 - register DT = targetRegX
						i_fx15(targetRegX);
						break;
					}
					case 0x18:
					{
						// fx18 - register ST = targetRegX
						i_fx18(targetRegX);
						break;
					}
					case 0x1e:
					{
						// fx1e - register i = register i + targetRegX
						i_fx1e(targetRegX);
						break;
					}
					case 0x29:
					{
						// fx29 - register i = location of sprite specified by targetRegX
						// (not fully sure what this means yet)
						i_fx29(targetRegX);
						break;
					}
					case 0x33:
					{
						// fx33 - store binary coded decimal representation of
						// targetRegX in memory locations i, i+1, i+2
						i_fx33(targetRegX);
						break;
					}
					case 0x55:
					{
						// fx55 - store registers v0 through targetRegX in memory
						// starting at location i
						i_fx55(targetRegX);

						break;
					}
					case 0x65:
					{
						// fx65 - read registers v0 through targetRegX from memory
						// starting at location i
						i_fx65(targetRegX);

						break;
					}
				}

				break;
			}
		}

		// increment pc by 2 bytes
		r_pc += 2;

		// if pc > 0xfff, exit program
		if (!validPC(r_pc))
		{
			wprintw(messageWin, "PC is %hx; exiting...\n", r_pc);
			wrefresh(messageWin);

			return;
		}
	}

	exitProgram(0, "Emulation complete");
}

void printUsage()
{
	printf("usage: chip8 [run|disassemble] file.rom [flags]\n");
	printf("flags:\n");
	printf("\t-s 120\tSets emulation speed in terms of cpu cycles/second (default: 300)\n");
	printf("\t--quirk-shift-result\tStore the result of a shift in vy instead of vx\n");
	printf("\t--quirk-change-i-during-save-load\t Modify the I register after fx55/fx65\n");
}

int main(int argc, char *argv[])
{
	char* command;
	char* filename;
	bool emuSpeedSet = false;
	unsigned int emuSpeed = 0;
	if (argc < 3)
	{
		printUsage();
		return -1;
	}
	
	command = argv[1];
	filename = argv[2];

	for (int i = 3; i < argc; i++)
	{
		if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
		{
			printUsage();
			return 1;
		}
		else if (strcmp(argv[i], "--speed") == 0 || strcmp(argv[i], "-s") == 0)
		{
			if (argc > i + 1)
			{
				emuSpeedSet = true;
				emuSpeed = (unsigned int)atoi(argv[++i]);
				if (emuSpeed > 1000000 || emuSpeed < 1)
				{
					printf("Enter emulation speed between 1 and 1000000\n");
					return 1;
				}
			}
			else
			{
				printf("Enter emulation speed in terms of cycles/second\n");
				return 1;
			}
		}
		else if (strcmp(argv[i], "--quirk-shift-result") == 0 || strcmp(argv[i], "-q1") == 0)
		{
			QUIRK_SHIFT_RESULT_IN_VY = true;
		}
		else if (strcmp(argv[i], "--quirk-change-i-during-save-load") == 0 || strcmp(argv[i], "-q2") == 0)
		{
			QUIRK_SET_I_AFTER_LOAD_STORE = true;
		}
		else
		{
			printUsage();
			return 1;
		}
	}

	// Initialize registers, memory
	initializeState();

	// read romfile into buffer
	unsigned char *buffer;
	size_t length;
	if (access(filename, R_OK) == -1)
	{
		printf("invalid filename\n");
		return 1;
	}
	readFile(filename, &buffer, &length);

	bool loadSuccess = loadRomToMemory(&buffer, length);
	if (!loadSuccess)
	{
		printf("%s", "error: ROM size greater than 0xc8f bytes\n");
		return 1;
	}

	int retval = 0;
	if (command[0] == 'r')
	{
		// start main program processing loop
		loop(emuSpeed);
		printf("Execution complete.\n");

		//if (memoryDump)
		//{
		//	dumpMem();
		//}
	}
	else if (command[0] == 'd')
	{
		disassemble(length);
	}
	else
	{
		printUsage();
		retval = -1;
	}

	free(buffer);
	exitProgram(retval, "");
}
