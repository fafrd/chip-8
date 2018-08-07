#ifndef STATE_H
#define STATE_H

#define STACK_SIZE 16

void initializeState();

// main memory
extern unsigned char* mem;

// ye olde stack
extern unsigned short* stack;

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

#endif
