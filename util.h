#ifndef UTIL_H
#define UTIL_H

unsigned char* getVxReg(unsigned char);
void dumpMem();
void dumpRegs();
void dumpKeyState();
bool xorToPoint(int, int, bool);
bool kbhit();
bool validPC(unsigned short);
void exitProgram(int, char*);

#endif
