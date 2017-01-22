#ifndef _BF_H_
#define _BF_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define DATA_BLOCK_SZ 999

#define COLOR_PC 1
#define COLOR_DC 2
#define COLOR_BP 3
#define COLOR_PC_ON_BP 4
#define COLOR_DC_ON_BP 5

#define HIT_BREAK_POINT -3

struct MACHINE_STATE_T{
	char* szTape;
	unsigned int uiProgramCounter;
	unsigned int uiDataCounter;
	unsigned int uTapeLen;
	unsigned int* puBreakPoints;
	unsigned int uBreakPointCount;
};

struct MACHINE_STATE_T* createMachine(char* szProgram); //creates machine with program string
int freeMachine(struct MACHINE_STATE_T* tMachine);

int evalStep(struct MACHINE_STATE_T* tMachine, int ignoreBP); //evaluates one step of bf machine

int addBreakPoint(struct MACHINE_STATE_T* tMachine, unsigned int uAddr);

int checkBreakPoint(struct MACHINE_STATE_T* tMachine, unsigned int uAddr);

int remBreakPoint(struct MACHINE_STATE_T* tMachine, unsigned int uAddr);

#endif