#ifndef UTIL_H
#define UTIL_H

#include <unistd.h>

unsigned char* getVxReg(unsigned char);
void dumpMem();
void dumpRegs();
void dumpKeyState();
bool xorToPoint(int, int, bool);
bool kbhit();
bool validPC(unsigned short);
useconds_t calculateDelay(unsigned int hz);
void exitProgram(int, char*);

#endif
