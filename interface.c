#include "interface.h"

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

int printWindow(struct WINDOW_T* destWin, char* szTape, unsigned int uTapeLen, struct MACHINE_STATE_T* tSrc){
	//window can either be 80 or 40 wide
	//if(destWin->uWidth != 80 || destWin->uWidth != 40)
	//	return -1;

	//box(destWin->tOutput,0,0);
	wclear(destWin->tOutput);

	if(!destWin->fIsVisible)
		return 1;

	int printLines;
	int printAmtPerLine;
	int i,x;

	switch(destWin->printType){
		case PRINT_TYPE_HEX:
			if(destWin->uWidth == 80){
				printAmtPerLine = 24;
				printLines = WINDOW_HEIGHT_FULL-1;
			}else{
				printAmtPerLine = 11;
				printLines = WINDOW_HEIGHT_FULL-1;
			}

			for(i=1;i<printLines;i++){
				if(destWin->uAddressView + (i-1)*printAmtPerLine > uTapeLen)
						break;
				printWindowLine(destWin->tOutput, PRINT_TYPE_HEX, i, printAmtPerLine, szTape, destWin->uAddressView + (i-1)*printAmtPerLine, tSrc);
				
			}

			break;
		case PRINT_TYPE_ASC:
			if(destWin->uWidth == 80){
				printAmtPerLine = 24;
				printLines = WINDOW_HEIGHT_FULL-1;
			}else{
				printAmtPerLine = 11;
				printLines = WINDOW_HEIGHT_FULL-1;
			}

			for(i=1;i<printLines;i++){
				if(destWin->uAddressView + (i-1)*printAmtPerLine > uTapeLen)
					break;
				printWindowLine(destWin->tOutput, PRINT_TYPE_ASC, i, printAmtPerLine, szTape, destWin->uAddressView + (i-1)*printAmtPerLine, tSrc);
			}

			break;
		case PRINT_TYPE_ASM:
			printAmtPerLine = 1;
			printLines = WINDOW_HEIGHT_FULL-1;
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

	box(destWin->tOutput,0,0);
	wrefresh(destWin->tOutput);
	return 0;
}

int printWindowLine(WINDOW* destWin, char printType, int y, int x, char* szTape, unsigned int uAddress, struct MACHINE_STATE_T* tSrc){
	char* tempOutLine;
	if(printType == PRINT_TYPE_HEX || printType == PRINT_TYPE_ASC)
		tempOutLine = (char*)malloc(x*6);
	else if(printType == PRINT_TYPE_ASM)
		tempOutLine = (char*)malloc(16);
	char* tempOutSegment = (char*)malloc(8);

	sprintf(tempOutLine, "%04x:", uAddress);

	mvwprintw(destWin, y, 1, tempOutLine);

	init_pair(COLOR_PC, COLOR_WHITE, COLOR_GREEN);
	init_pair(COLOR_DC, COLOR_WHITE, COLOR_BLUE);
	init_pair(COLOR_BP, COLOR_WHITE, COLOR_RED);
	init_pair(COLOR_PC_ON_BP, COLOR_GREEN, COLOR_RED);
	init_pair(COLOR_DC_ON_BP, COLOR_BLUE, COLOR_RED);

	int i;
	for(i=0;i<x;i++){
		waddch(destWin, ' ');
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
		switch(printType){
			default:
			case PRINT_TYPE_HEX:
				sprintf(tempOutSegment, "%02x", szTape[uAddress+i]);
				wprintw(destWin, tempOutSegment);
				break;
			case PRINT_TYPE_ASC:
				if(isgraph(szTape[uAddress+i]))
					sprintf(tempOutSegment, " %c", szTape[uAddress+i]);
				else
					sprintf(tempOutSegment, "  ", szTape[uAddress+i]);
				wprintw(destWin, tempOutSegment);
				break;
			case PRINT_TYPE_ASM:
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
				wprintw(destWin, tempOutSegment);
				break;
		}

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

		//strcat(tempOutLine, tempOutSegment);
	}
	//mvwprintw(destWin, y, 1, tempOutLine);

	free(tempOutLine);
	free(tempOutSegment);

	return i;
}

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