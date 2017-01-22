#include "interface.h"

//returns a blank expanded window struct
struct WINDOW_T* addWindowView(int winHeight, int winWidth){	
	WINDOW* tOut = newwin(winHeight, winWidth, 0, 0);

	struct WINDOW_T* destWin = (struct WINDOW_T*)malloc(sizeof(struct WINDOW_T));
	
	destWin->tOutput = tOut;

	destWin->uHeight = winHeight;
	destWin->uWidth = winWidth;

	destWin->fIsVisible = 1;
	destWin->printType = PRINT_TYPE_HEX;

	destWin->uAddressView = 0;

	//box(tOut,0,0);
	//wborder(tOut, '#', '#', '#', '#', '#', '#', '#', '#');

	wrefresh(tOut);

	return destWin;	
}

//prints out contents of tape into selected window
int printWindow(struct WINDOW_T* destWin, char* szTape, unsigned int uTapeLen, struct MACHINE_STATE_T* tSrc){

	int printLines;
	int printAmtPerLine;
	int i,x;

	//clear window before proceeding
	wclear(destWin->tOutput);

	//if window isn't visible, don't bother calculating anything and just leave
	if(!destWin->fIsVisible)
		return 1;

	//switch for window print type
	switch(destWin->printType){
		//print out tape byte by byte as hexadecimal integers
		case PRINT_TYPE_HEX:
			//if window is a full length window print 24 bytes per line
			if(destWin->uWidth == WINDOW_WIDTH_FULL){
				printAmtPerLine = 24;
				printLines = WINDOW_HEIGHT_FULL-1;
			}else{
				//otherwise we only have room for 11
				printAmtPerLine = 11;
				printLines = WINDOW_HEIGHT_FULL-1;
			}

			//for each line
			for(i=1;i<printLines;i++){
				//if there's nothing left on the tape exit
				if(destWin->uAddressView + (i-1)*printAmtPerLine > uTapeLen)
						break;
				//print line into window
				printWindowLine(destWin->tOutput, PRINT_TYPE_HEX, i, printAmtPerLine, szTape, destWin->uAddressView + (i-1)*printAmtPerLine, tSrc);
				
			}
			break;
			//byte by byte, print out ONLY cells that have a graphical ascii representation
		case PRINT_TYPE_ASC:
			//same as before- full width we can fit 24 characters, half width or other we can fit 11S
			if(destWin->uWidth == WINDOW_WIDTH_FULL){
				printAmtPerLine = 24;
				printLines = WINDOW_HEIGHT_FULL-1;
			}else{
				printAmtPerLine = 11;
				printLines = WINDOW_HEIGHT_FULL-1;
			}

			//again, same as before
			for(i=1;i<printLines;i++){
				if(destWin->uAddressView + (i-1)*printAmtPerLine > uTapeLen)
					break;
				printWindowLine(destWin->tOutput, PRINT_TYPE_ASC, i, printAmtPerLine, szTape, destWin->uAddressView + (i-1)*printAmtPerLine, tSrc);
			}
			break;
			//print single line at a time as english ASM instructions
		case PRINT_TYPE_ASM:
			//only one byte (instruction) per line
			printAmtPerLine = 1;
			printLines = WINDOW_HEIGHT_FULL-1;
			//the rest is same as before
			for(i=1;i<printLines;i++){
				if(destWin->uAddressView + (i-1) > uTapeLen)
					break;
				else
					printWindowLine(destWin->tOutput, PRINT_TYPE_ASM, i, printAmtPerLine, szTape, destWin->uAddressView+(i-1), tSrc);
			}
			break;
		default:
			break;
	}

	//draw a box around the window and project it from the buffer onto the screen
	box(destWin->tOutput,0,0);
	wrefresh(destWin->tOutput);
	return 0;
}

//helper function used by printWindow() for printing lines
int printWindowLine(WINDOW* destWin, char printType, int y, int x, char* szTape, unsigned int uAddress, struct MACHINE_STATE_T* tSrc){
	
	char* tempOutLine;

	//if we're printing in hex or ascii...
	if(printType == PRINT_TYPE_HEX || printType == PRINT_TYPE_ASC)
		//allocate more memory
		tempOutLine = (char*)malloc(x*6);
	else if(printType == PRINT_TYPE_ASM)
		//if we're printing in assembly we only need a few bytes really
		tempOutLine = (char*)malloc(16);

	//temprorary segment for printing out
	char* tempOutSegment = (char*)malloc(8);

	//put hex address into string
	sprintf(tempOutLine, "%04x:", uAddress);

	//print address first
	mvwprintw(destWin, y, 1, tempOutLine);

	//initialize our color pairs
	init_pair(COLOR_PC, COLOR_WHITE, COLOR_GREEN);
	init_pair(COLOR_DC, COLOR_WHITE, COLOR_BLUE);
	init_pair(COLOR_BP, COLOR_WHITE, COLOR_RED);
	init_pair(COLOR_PC_ON_BP, COLOR_GREEN, COLOR_RED);
	init_pair(COLOR_DC_ON_BP, COLOR_BLUE, COLOR_RED);

	//for each line, draw X elements (bytes)
	int i;
	for(i=0;i<x;i++){
		//space out elements
		waddch(destWin, ' ');
		//set color for program counter, data counter, breakpoints, and overlap points
		if(uAddress+i == tSrc->uiProgramCounter)
			wattron(destWin, COLOR_PAIR(COLOR_PC));
		if(uAddress+i == tSrc->uiDataCounter)
			wattron(destWin, COLOR_PAIR(COLOR_DC));
		if(checkBreakPoint(tSrc,uAddress+i))
			wattron(destWin, COLOR_PAIR(COLOR_BP));
		if(uAddress+i == tSrc->uiProgramCounter && checkBreakPoint(tSrc,uAddress+i))
			wattron(destWin, COLOR_PAIR(COLOR_PC_ON_BP));
		if(uAddress+i == tSrc->uiDataCounter && checkBreakPoint(tSrc,uAddress+i))
			wattron(destWin, COLOR_PAIR(COLOR_DC_ON_BP));
		//switch for print format of window
		switch(printType){
			//print hex by default
			default:
			//byte by byte hex out
			case PRINT_TYPE_HEX:
				//convert to hexadecimal and print out
				sprintf(tempOutSegment, "%02x", szTape[uAddress+i]);
				wprintw(destWin, tempOutSegment);
				break;
			//byte by byte graphical ASCII out
			case PRINT_TYPE_ASC:
				//if graphical, print out
				if(isgraph(szTape[uAddress+i]))
					sprintf(tempOutSegment, " %c", szTape[uAddress+i]);
				else
					//otherwise, print out blank space
					sprintf(tempOutSegment, "  ", szTape[uAddress+i]);
				wprintw(destWin, tempOutSegment);
				break;
			//byte by byte instruction out
			case PRINT_TYPE_ASM:
				//switch for ASCII -> instruction
				switch(szTape[uAddress+i]){
					case '+':
						sprintf(tempOutSegment, "INCREMENT");
						break;
					case '-':
						sprintf(tempOutSegment, "DECREMENT");
						break;
					case '>':
						sprintf(tempOutSegment, "MOVE_L");
						break;
					case '<':
						sprintf(tempOutSegment, "MOVE_R");
						break;
					case '[':
						sprintf(tempOutSegment, "JDZ");
						break;
					case ']':
						sprintf(tempOutSegment, "JUNZ");
						break;
					case '.':
						sprintf(tempOutSegment, "OUT");
						break;
					case ',':
						sprintf(tempOutSegment, "IN");
						break;
				}
				//print out
				wprintw(destWin, tempOutSegment);
				break;
		}

		//turn off our colors
		if(uAddress+i == tSrc->uiProgramCounter)
			wattroff(destWin, COLOR_PAIR(COLOR_PC));
		if(uAddress+i == tSrc->uiDataCounter)
			wattroff(destWin, COLOR_PAIR(COLOR_DC));
		if(checkBreakPoint(tSrc,uAddress+i))
			wattroff(destWin, COLOR_PAIR(COLOR_BP));
		if(uAddress+i == tSrc->uiProgramCounter && checkBreakPoint(tSrc,uAddress+i))
			wattroff(destWin, COLOR_PAIR(COLOR_PC_ON_BP));
		if(uAddress+i == tSrc->uiDataCounter && checkBreakPoint(tSrc,uAddress+i))
			wattroff(destWin, COLOR_PAIR(COLOR_DC_ON_BP));
	}

	//free lines, return elements printed out
	free(tempOutLine);
	free(tempOutSegment);

	return i;
}

//swap quickly to one of four preset views based on a two window display
int changeView(int state, struct WINDOW_T* primaryWin, struct WINDOW_T* secondaryWin){
	//states:
	//	1: primary only
	//  2: primary, secondary
	//  3: secondary, primary
	//  4: secondary only
	switch(state){
		case VIEW_PRIMARY:
			wresize(primaryWin->tOutput, WINDOW_HEIGHT_FULL, WINDOW_WIDTH_FULL);
			mvwin(primaryWin->tOutput,0,0);
			primaryWin->fIsVisible = 1;
			primaryWin->uWidth = WINDOW_WIDTH_FULL;
			secondaryWin->fIsVisible = 0;
			break;
		case VIEW_BOTH:
			wresize(primaryWin->tOutput, WINDOW_HEIGHT_FULL, WINDOW_WIDTH_HALF);
			wresize(secondaryWin->tOutput, WINDOW_HEIGHT_FULL, WINDOW_WIDTH_HALF);
			mvwin(primaryWin->tOutput, 0, 0);
			mvwin(secondaryWin->tOutput, 0, WINDOW_WIDTH_HALF);
			primaryWin->fIsVisible = 1;
			primaryWin->uWidth = WINDOW_WIDTH_HALF;
			secondaryWin->fIsVisible = 1;
			secondaryWin->uWidth = WINDOW_WIDTH_HALF;
			break;
		case VIEW_SECONDARY:
			wresize(secondaryWin->tOutput, WINDOW_HEIGHT_FULL, WINDOW_WIDTH_FULL);
			mvwin(secondaryWin->tOutput,0,0);
			primaryWin->fIsVisible = 0;
			secondaryWin->fIsVisible = 1;
			secondaryWin->uWidth = WINDOW_WIDTH_FULL;
			break;
		case VIEW_BOTH_FLIP:
			wresize(primaryWin->tOutput, WINDOW_HEIGHT_FULL, WINDOW_WIDTH_HALF);
			wresize(secondaryWin->tOutput, WINDOW_HEIGHT_FULL, WINDOW_WIDTH_HALF);
			mvwin(primaryWin->tOutput, 0, WINDOW_WIDTH_HALF);
			mvwin(secondaryWin->tOutput, 0, 0);
			primaryWin->fIsVisible = 1;
			primaryWin->uWidth = WINDOW_WIDTH_HALF;
			secondaryWin->fIsVisible = 1;
			secondaryWin->uWidth = WINDOW_WIDTH_HALF;
			break;
		default:
			return 1;
	}
	return 0;
}