#include "bf.h"

//function for creating bf machine
struct MACHINE_STATE_T* createMachine(char* szProgram){
	//if program text is empty we return nothing
	if(!szProgram)
		return 0;

	//allocate memory for machine
	struct MACHINE_STATE_T* newMachine = (struct MACHINE_STATE_T*)malloc(sizeof(struct MACHINE_STATE_T));

	//size of the tape = program text + default data block (more than enough for most bf programs)
	newMachine->szTape = (char*)calloc(strlen(szProgram)+DATA_BLOCK_SZ,1);

	//copies program text onto the machine's tape
	memcpy(newMachine->szTape, szProgram, strlen(szProgram));

	//resets all counters and pointers
	newMachine->uiProgramCounter = 0;
	newMachine->uiDataCounter = strlen(szProgram);
	newMachine->uBreakPointCount = 0;

	//calculates tape length
	newMachine->uTapeLen = strlen(szProgram) + DATA_BLOCK_SZ;

	//returns a 'ready to go' brainfuck machine
	return newMachine;
}

int evalStep(struct MACHINE_STATE_T* srcMachine, int ignoreBP){

	//counters for looping instructions
	unsigned int uiLoopProgramCounter;
	unsigned int uiLoopDepth=0;

	//if we're not ignoring breakpoints and the current address has one set...
	if(checkBreakPoint(srcMachine, srcMachine->uiProgramCounter) && !ignoreBP){
		//...we do nothing and return that we hit a break point
		return HIT_BREAK_POINT;
	}

	//switch for instruction located on the tape at offset indicated by the program counter
	switch(srcMachine->szTape[srcMachine->uiProgramCounter]){
		case '+':
			//increment cell on the tape at offset indicated by the data counter
			srcMachine->szTape[srcMachine->uiDataCounter]++;
			break;
		case '-':
			//decrement cell on the tape at offset indicated by the data counter
			srcMachine->szTape[srcMachine->uiDataCounter]--;
			break;
		case '>':
			//increments data counter
			srcMachine->uiDataCounter++;
			break;
		case '<':
			//decrements data counter
			srcMachine->uiDataCounter--;
			break;
		case '[':
			//if the cell on the tape at the offset indicated by the data counter is zero...
			if(!srcMachine->szTape[srcMachine->uiDataCounter]){
				//...we search for the corresponding end of the loop
				for(uiLoopProgramCounter = srcMachine->uiProgramCounter;;uiLoopProgramCounter++){
					//if we pass another loop, increase the scope depth we're at
					if(srcMachine->szTape[uiLoopProgramCounter] == '[')
						uiLoopDepth++;
					//if we pass a closing bracket of a loop, go up one scope depth level
					if(srcMachine->szTape[uiLoopProgramCounter] == ']'){
						uiLoopDepth--;
						//if we're at the depth we started at set program counter to loop program counter
						if(!uiLoopDepth){
							srcMachine->uiProgramCounter = uiLoopProgramCounter;
							return 0;
						}
					}

				}
			}
			break;
		case ']':
			//if the cell on the tape at the offset indicated by the data counter is non-zero...
			if(srcMachine->szTape[srcMachine->uiDataCounter]){
				//...look for the opening loop bracket
				for(uiLoopProgramCounter = srcMachine->uiProgramCounter;;uiLoopProgramCounter--){
					//if we pass another closing loop bracket, increase our depth
					if(srcMachine->szTape[uiLoopProgramCounter] == ']')
						uiLoopDepth++;
					//decrease depth when passing opening loop bracket
					if(srcMachine->szTape[uiLoopProgramCounter] == '['){
						uiLoopDepth--;
						//if loop depth is zero, set program counter to loop program counter
						if(!uiLoopDepth){
							srcMachine->uiProgramCounter = uiLoopProgramCounter;
							return 0;
						}
					}
				}
			}
			break;
		case '.':
			//increment program counter and return character to print
			srcMachine->uiProgramCounter++;
			return (char)srcMachine->szTape[srcMachine->uiDataCounter];
			break;
		case ',':
			break;
		default:
			//anything that isn't an instruction is by default considered to be a comment
			break;
	}

	//increase PC before returning
	srcMachine->uiProgramCounter++;

	return 0;
}

//frees memory allocated by a machine
int freeMachine(struct MACHINE_STATE_T* tMachine){
	if(tMachine){
		if(tMachine->szTape)
			free(tMachine->szTape);
		free(tMachine);
	}
}

//adds breakpoint to machine at uAddr
int addBreakPoint(struct MACHINE_STATE_T* tMachine, unsigned int uAddr){
	//if the machine has no breakpoints yet
	if(!tMachine->uBreakPointCount)
	{
		//allocate space for a breakpoint
		tMachine->uBreakPointCount++;
		tMachine->puBreakPoints = (unsigned int*)malloc(sizeof(unsigned int)*tMachine->uBreakPointCount);
	}else{
		//reallocate space for breakpoints if adding another one to an existing breakpoint list
		tMachine->puBreakPoints = (unsigned int*)realloc(tMachine->puBreakPoints, sizeof(unsigned int)*(++tMachine->uBreakPointCount));
	}

	//add breakpoint to list
	tMachine->puBreakPoints[tMachine->uBreakPointCount-1] = uAddr;

	return 0;
}

//check if breakpoint uAddr exists in tMachine, returns 1 if one exists and 0 if not
int checkBreakPoint(struct MACHINE_STATE_T* tMachine, unsigned int uAddr){
	int i;
	for(i = 0;i < tMachine->uBreakPointCount;i++){
		if(tMachine->puBreakPoints[i] == uAddr)
			return 1;
	}
	return 0;
}

//removes breakpoint uAddr from the machine
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