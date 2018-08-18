#ifndef STATE_H
#define STATE_H

#include <stdbool.h>
#include <ncurses.h>

#define STACK_SIZE 16

void initializeState();
void* updateTimers(void*);
unsigned char mapKey(int);
void updateKeyState(unsigned char);
void clearAllKeyStates();
bool getStateForKey(unsigned char);
void createDrawWindow();
void createMessageWindow();
void createRegisterWindow();
void clearScreen();

// main memory
extern unsigned char* mem;

// ye olde stack
extern unsigned short* stack;

// the screen is an array of boolean values
bool* screen;

// ncurses windows
WINDOW* drawWin;
WINDOW* messageWin;
WINDOW* registerWin;

// general purpose registers
extern unsigned short r_i;
extern unsigned char r_v0;
extern unsigned char r_v1;
extern unsigned char r_v2;
extern unsigned char r_v3;
extern unsigned char r_v4;
extern unsigned char r_v5;
extern unsigned char r_v6;
extern unsigned char r_v7;
extern unsigned char r_v8;
extern unsigned char r_v9;
extern unsigned char r_va;
extern unsigned char r_vb;
extern unsigned char r_vc;
extern unsigned char r_vd;
extern unsigned char r_ve;
extern unsigned char r_vf;

// special registers
extern unsigned short r_pc; // program counter
extern unsigned char r_sp; // stack pointer
extern unsigned char r_dt; // delay timer
extern unsigned char r_st; // sound timer

// key presses
extern bool key0;
extern bool key1;
extern bool key2;
extern bool key3;
extern bool key4;
extern bool key5;
extern bool key6;
extern bool key7;
extern bool key8;
extern bool key9;
extern bool keyA;
extern bool keyB;
extern bool keyC;
extern bool keyD;
extern bool keyE;
extern bool keyF;

#endif
