#include "bf.h"

struct MACHINE_STATE_T* createMachine(char* szProgram){
	if(!szProgram)
		return 0;

	struct MACHINE_STATE_T* newMachine = (struct MACHINE_STATE_T*)malloc(sizeof(struct MACHINE_STATE_T));

	newMachine->szTape = (char*)calloc(strlen(szProgram)+DATA_BLOCK_SZ,1);

	memcpy(newMachine->szTape, szProgram, strlen(szProgram));

	newMachine->uiProgramCounter = 0;
	newMachine->uiDataCounter = strlen(szProgram);
	newMachine->uBreakPointCount = 0;

	newMachine->uTapeLen = strlen(szProgram) + DATA_BLOCK_SZ;

	return newMachine;
}

int evalStep(struct MACHINE_STATE_T* srcMachine, int ignoreBP){

	unsigned int uiLoopProgramCounter;
	unsigned int uiLoopDepth=0;

	if(checkBreakPoint(srcMachine, srcMachine->uiProgramCounter) && !ignoreBP){ // we hit a breakpoint
		//remBreakPoint(srcMachine, srcMachine->uiProgramCounter);
		return HIT_BREAK_POINT;
	}

	switch(srcMachine->szTape[srcMachine->uiProgramCounter]){
		case '+':
			srcMachine->szTape[srcMachine->uiDataCounter]++;
			break;
		case '-':
			srcMachine->szTape[srcMachine->uiDataCounter]--;
			break;
		case '>':
			srcMachine->uiDataCounter++;
			break;
		case '<':
			srcMachine->uiDataCounter--;
			break;
		case '[':
			if(!srcMachine->szTape[srcMachine->uiDataCounter]){
				for(uiLoopProgramCounter = srcMachine->uiProgramCounter;;uiLoopProgramCounter++){
					if(srcMachine->szTape[uiLoopProgramCounter] == '[')
						uiLoopDepth++;
					if(srcMachine->szTape[uiLoopProgramCounter] == ']'){
						uiLoopDepth--;
						if(!uiLoopDepth){
							srcMachine->uiProgramCounter = uiLoopProgramCounter;
							return 0;
						}
					}

				}
			}
			break;
		case ']':
			if(srcMachine->szTape[srcMachine->uiDataCounter]){
				for(uiLoopProgramCounter = srcMachine->uiProgramCounter;;uiLoopProgramCounter--){
					if(srcMachine->szTape[uiLoopProgramCounter] == ']')
						uiLoopDepth++;
					if(srcMachine->szTape[uiLoopProgramCounter] == '['){
						uiLoopDepth--;
						if(!uiLoopDepth){
							srcMachine->uiProgramCounter = uiLoopProgramCounter;
							return 0;
						}
					}
				}
			}
			break;
		case '.':
			srcMachine->uiProgramCounter++;
			return (char)srcMachine->szTape[srcMachine->uiDataCounter];
			break;
		case ',':
			break;
		default:
			break;
	}

	srcMachine->uiProgramCounter++;

	return 0;
}

int freeMachine(struct MACHINE_STATE_T* tMachine){
	if(tMachine){
		if(tMachine->szTape)
			free(tMachine->szTape);
		free(tMachine);
	}
}

int addBreakPoint(struct MACHINE_STATE_T* tMachine, unsigned int uAddr){
	if(!tMachine->uBreakPointCount)
	{
		tMachine->uBreakPointCount++;
		tMachine->puBreakPoints = (unsigned int*)malloc(sizeof(unsigned int)*tMachine->uBreakPointCount);
	}else{
		tMachine->puBreakPoints = (unsigned int*)realloc(tMachine->puBreakPoints, sizeof(unsigned int)*(++tMachine->uBreakPointCount));
	}

	tMachine->puBreakPoints[tMachine->uBreakPointCount-1] = uAddr;

	return 0;
}

int checkBreakPoint(struct MACHINE_STATE_T* tMachine, unsigned int uAddr){
	int i;
	for(i = 0;i < tMachine->uBreakPointCount;i++){
		if(tMachine->puBreakPoints[i] == uAddr)
			return 1;
	}
	return 0;
}

int remBreakPoint(struct MACHINE_STATE_T* tMachine, unsigned int uAddr){
	int i;
	for(i = 0; i < tMachine->uBreakPointCount; i++){
		if(tMachine->puBreakPoints[i] == uAddr){
			memcpy(&tMachine->puBreakPoints[i], &tMachine->puBreakPoints[i+1], sizeof(unsigned int)*(tMachine->uBreakPointCount-i));
			tMachine->uBreakPointCount--;
			return 0;
		}
	}
	return 1;
}