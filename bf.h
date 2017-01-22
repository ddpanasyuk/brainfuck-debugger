#ifndef _BF_H_
#define _BF_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

//default size of tape block for non-program data
#define DATA_BLOCK_SZ 999

//different color pairs for debugging
#define COLOR_PC 1
#define COLOR_DC 2
#define COLOR_BP 3
#define COLOR_PC_ON_BP 4
#define COLOR_DC_ON_BP 5

#define HIT_BREAK_POINT -3

//brainfuck machine state struct
struct MACHINE_STATE_T{
	//tape for program and data text
	char* szTape;
	//instruction counter and data pointer
	unsigned int uiProgramCounter;
	unsigned int uiDataCounter;
	//length of tape (bytes)
	unsigned int uTapeLen;
	//list of breakpoints
	unsigned int* puBreakPoints;
	unsigned int uBreakPointCount;
};

//takes input program text and returns a set up brainfuck machine
struct MACHINE_STATE_T* createMachine(char* szProgram);

//frees machine resources
int freeMachine(struct MACHINE_STATE_T* tMachine);

//evaluates one instruction cycle of brainfuck machine
int evalStep(struct MACHINE_STATE_T* tMachine, int ignoreBP);

//add a breakpoint at addr uAddr
int addBreakPoint(struct MACHINE_STATE_T* tMachine, unsigned int uAddr);

//check to see if a breakpoint exists at addr uAddr
int checkBreakPoint(struct MACHINE_STATE_T* tMachine, unsigned int uAddr);

//remove breakpoint at addr uAddr if one exists
int remBreakPoint(struct MACHINE_STATE_T* tMachine, unsigned int uAddr);

#endif