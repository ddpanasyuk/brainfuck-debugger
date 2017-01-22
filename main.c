#include "main.h"

int main(int argc, char** argv){

	FILE* f;
	int fileSz;
	char* szText;

	//check for sufficient arguments
	if(argc == 3){
		//if trying to open from a file
		if(!strcmp(argv[1], "-f")){
			f = fopen(argv[2], "r+b");
			//check if file is valid
			if(f){
				fseek(f, 0, SEEK_END);
				fileSz = ftell(f);
				rewind(f);

				szText = (char*)malloc(fileSz);

				fread(szText,1,fileSz,f);
				fclose(f);

				//read from file and pass buffer to our work loop
				workLoop(szText);
				free(szText);
				return 0;
			}else{
				//if it isn't print error, exit
				printf("File \"%s\" could not be found or opened.\n",argv[2]);
				return -1;
			}
		//if argument is direct input pass argument string directly to work loop
		}else if(!strcmp(argv[1], "-d")){
			return workLoop(strdup(argv[2]));
		}else
			goto inputError;
	}else{
		//oops! input correct inputs
		inputError:
			printf("bdg: Insufficient arguments \n Usage: bdg <source method> <source>\n");
			printf("Source methods: -f to load from file, -d to load directly\nSource: Filename or string of bf code\n");
			return -1;
	}

	return 0;
}

//main loop for program
int workLoop(char* szText){
	int cIn;
	int viewState = 1;
	int outX,outY;
	int selectedWindow = 0;
	unsigned int uBreakPoint;
	int ignoreBP = 0;
	int fIsRunning = 0;
	int toAddress;
	char toChar;
	WINDOW* helpWin;

	//initialize the screen
	initscr();
	cbreak();
	noecho();
	start_color();


	//initalize windows and machine
	struct WINDOW_T* primaryWin = addWindowView(WINDOW_HEIGHT_FULL, WINDOW_WIDTH_HALF);
	struct WINDOW_T* secondaryWin = addWindowView(WINDOW_HEIGHT_FULL, WINDOW_WIDTH_HALF);
	//WINDOW* ctrlWin = newwin(80-WINDOW_HEIGHT_FULL, WINDOW_WIDTH_FULL, WINDOW_HEIGHT_FULL, 0);
	WINDOW* getWin = newwin(4, WINDOW_WIDTH_FULL, WINDOW_HEIGHT_FULL, 0);
	keypad(getWin, TRUE);
	struct MACHINE_STATE_T* tMachine = createMachine(szText);

	//print initial screen then wait
	changeView(viewState, primaryWin, secondaryWin);
	printWindow(primaryWin, tMachine->szTape, tMachine->uTapeLen, tMachine);
	printWindow(secondaryWin, tMachine->szTape, tMachine->uTapeLen, tMachine);

	//coordinates for output window
	outX = 1;
	outY = 1;

	while(1){
		//if program is running in real time
		if(fIsRunning){
			//evaluate step
			cIn = evalStep(tMachine, ignoreBP);
			//if non-zero return, print it in output box
			if(cIn > 0){
				if(outX > WINDOW_HEIGHT_FULL-2){
					wclear(getWin);
					outX = 1;
				}
				mvwaddch(getWin, outY, outX++, cIn);
			//otherwise it's most likely a break point
			}else if(cIn == HIT_BREAK_POINT){
				//alert user, print breakpoint location
				beep();
				flash();
				mvwprintw(getWin, 2,1,"Hit breakpoint at 0x%x", tMachine->uiProgramCounter);
				
				//ignore breakpoint on next loop around, stop running
				ignoreBP = 1;
				fIsRunning = 0;

				//wait for user input, then clear window and skip to label (SPAGHETTI)
				wgetch(getWin);
				wclear(getWin);
				goto skipBP;
			}
			//after one breakpoint is skipped, we can count future breakpoints
			ignoreBP = 0;
		}else{
			//stepping through program

			//redraw box and wait for input
			box(getWin,0,0);
			cIn = wgetch(getWin);
			
			//switch based on input
			switch(cIn){
				//if 'q' we exit program
				case 'q':
					free(primaryWin);
					free(secondaryWin);
					endwin();
					return 0;
				//tab changes the display state
				case '\t':
					viewState++;
					if(viewState > 4)
						viewState = 1;
					changeView(viewState, primaryWin, secondaryWin);
					break;
				//r switches from stepthrough to running
				case 'r':
					fIsRunning=1;
					break;
				//s is used for stepping from instruction to instruction
				case 's':
					cIn = evalStep(tMachine, ignoreBP);
					if(cIn > 0){
						if(outX > WINDOW_HEIGHT_FULL-2){
							wclear(getWin);
							outX = 1;
						}
						mvwaddch(getWin, outY, outX++, cIn);
					}else if(cIn == HIT_BREAK_POINT){
						beep();
						flash();
						mvwprintw(getWin, 2,1,"Hit breakpoint at 0x%x", tMachine->uiProgramCounter);
						ignoreBP = 1;
						//wrefresh(getWin);
						wgetch(getWin);
						wclear(getWin);
						goto skipBP;
					}
					ignoreBP = 0;
					skipBP:
						break;
				//g is used for GOTO, changes program counter
				case 'g':
					echo();
					mvwprintw(getWin,2,1, "Goto address: 0x");
					wscanw(getWin, "%x", &tMachine->uiProgramCounter);
					noecho();
					wgetch(getWin);
					wclear(getWin);
					break;
				//shift + G is used for changing the data counter
				case 'G':
					echo();
					mvwprintw(getWin,2,1, "Move pointer to address: 0x");
					wscanw(getWin, "%x", &tMachine->uiDataCounter);
					noecho();
					wgetch(getWin);
					wclear(getWin);
					break;
				//v and V are used for altering the print format of the primary and secondary winndows
				case 'v':
					primaryWin->printType++;
					if(primaryWin->printType > PRINT_TYPE_ASM)
						primaryWin->printType = PRINT_TYPE_HEX;
					break;
				case 'V':
					secondaryWin->printType++;
					if(secondaryWin->printType > PRINT_TYPE_ASM)
						secondaryWin->printType = PRINT_TYPE_HEX;
					break;
				//set breakpoints
				case 'b':
					echo();
					mvwprintw(getWin,2,1, "Set breakpoint at: 0x");
					wscanw(getWin, "%x", &uBreakPoint);
					noecho();
					wgetch(getWin);
					addBreakPoint(tMachine, uBreakPoint);
					wclear(getWin);
					break;
				//write directly to cell
				case 'w':
					echo();
					mvwprintw(getWin,2,1, "Write to address 0x");
					wscanw(getWin, "%x", &toAddress);
					mvwprintw(getWin,2,1, "Data to write to address: 0x");
					wscanw(getWin, "%x", &cIn);
					tMachine->szTape[toAddress] = (char)cIn;
					noecho();
					wgetch(getWin);
					wclear(getWin);
					break;
				//scroll up through addresses in selected window
				case KEY_UP:
					if(selectedWindow){
						if(secondaryWin->uAddressView == 0)
							break;
						else
							secondaryWin->uAddressView--;
						break;
					}else{
						if(primaryWin->uAddressView == 0)
							break;
						else
							primaryWin->uAddressView--;
					}
					break;
				//scroll down through addresses in selected window
				case KEY_DOWN:
					if(selectedWindow){
						if(secondaryWin->uAddressView+1 >= tMachine->uTapeLen)
							break;
						else
							secondaryWin->uAddressView++;
					}else{
						if(primaryWin->uAddressView+1 >= tMachine->uTapeLen)
							break;
						else
							primaryWin->uAddressView++;
					}
					break;
				//select left window
				case KEY_LEFT:
					selectedWindow = 0;
					break;
				//select right window
				case KEY_RIGHT:
					selectedWindow = 1;
					break;
				//help window
				case 'h':
					helpWin = newwin(25, 80, 0, 0);
					mvwprintw(helpWin, 0, 0, "bdg - a simple approach to brainfuck debugging and programming");
					mvwprintw(helpWin, 1, 0, "made by daniel panasyuk for hackucsc 2017");
					mvwprintw(helpWin, 2, 0, "<TAB> - cycles through different styles of window displays");
					mvwprintw(helpWin, 3, 0, "<v> - changes output format of primary window");
					mvwprintw(helpWin, 4, 0, "<SHIFT+v> - changes output format of secondary window");
					mvwprintw(helpWin, 5, 0, "<KEY UP/DOWN> - scrolls up/down through addresses on selected window");
					mvwprintw(helpWin, 6, 0, "<KEY LEFT/RIGHT> - selects primary/secondary window");
					mvwprintw(helpWin, 7, 0, "<g> - Write to PC (goto)");
					mvwprintw(helpWin, 8, 0, "<SHIFT+g> - Write to data pointer");
					mvwprintw(helpWin, 9, 0, "<b> - Set/Remove breakpoint");
					mvwprintw(helpWin, 10, 0, "<w> - Write to cell");
					mvwprintw(helpWin, 11, 0, "<r> - Run/Continue");
					mvwprintw(helpWin, 12, 0, "<s> - Step through to next instruction");
					mvwprintw(helpWin, 13, 0, "<h> - This page");
					mvwprintw(helpWin, 14, 0, "<q> - Exit program");
					wrefresh(helpWin);
					wclear(helpWin);
					wgetch(getWin);
				default:
					break;
			}
		}
		//print primary and secondary windows before exiting
		printWindow(primaryWin, tMachine->szTape, tMachine->uTapeLen, tMachine);
		printWindow(secondaryWin, tMachine->szTape, tMachine->uTapeLen, tMachine);
	}

	//end curses mode
	free(primaryWin);
	free(secondaryWin);
	endwin();
	return 0;
}