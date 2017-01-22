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
				return workLoop(szText);
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


	/*
	struct MACHINE_STATE_T* tempMachine = createMachine("++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.!");

	//while(tempMachine->szTape[tempMachine->uiProgramCounter] != '!')
	//	evalStep(tempMachine);


	initscr();
	cbreak();
	noecho();

	WINDOW* getWin = newwin(0,0,WINDOW_HEIGHT_FULL,0);
	keypad(getWin, TRUE);
	//workLoop();

	struct WINDOW_T* tempWin = addWindowView(WINDOW_HEIGHT_FULL, WINDOW_WIDTH_HALF);
	struct WINDOW_T* secondWin = addWindowView(WINDOW_HEIGHT_FULL, WINDOW_WIDTH_HALF);
	secondWin->uAddressView = 0x10;
	printWindow(tempWin, tempMachine->szTape, tempMachine->uTapeLen);
	mvwin(secondWin->tOutput, 0, 40);

	char cIn;
	while(tempMachine->szTape[tempMachine->uiProgramCounter] != '!'){
		cIn = wgetch(getWin);
		switch(cIn){
			case 'q':
				return 0;
			case '1':
				changeView(1,tempWin,secondWin);
				break;
			case '2':
				changeView(2,tempWin,secondWin);
				break;
			case '3':
				changeView(3,tempWin,secondWin);
				break;
			case '4':
				changeView(4,tempWin,secondWin);
				break;
			default:
				evalStep(tempMachine);
				break;
		}	
		printWindow(tempWin, tempMachine->szTape, tempMachine->uTapeLen);
		printWindow(secondWin, tempMachine->szTape, tempMachine->uTapeLen);
	}

	//char cIn;
	//int currState = 1;

	/*int i;
	for(i=1; i<5; i++){
		changeView(i, tempWin, secondWin);
		printWindow(tempWin, tempMachine->szTape, tempMachine->uTapeLen);
		printWindow(secondWin, tempMachine->szTape, tempMachine->uTapeLen);
		wgetch(getWin);
	}
	*/

/*
	freeMachine(tempMachine);
	
	wgetch(getWin);

	endwin();
	*/
	//workLoop();
	return 0;
}

int workLoop(char* szText){
	int cIn;
	int viewState = 1;
	int outX,outY;
	int selectedWindow = 0;
	unsigned int uBreakPoint;
	int ignoreBP = 0;
	int fIsRunning = 0;

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
	secondaryWin->uAddressView = 0x10;
	printWindow(secondaryWin, tMachine->szTape, tMachine->uTapeLen, tMachine);
	//wprintw(ctrlWin, "<TAB>: change window view <q>: exit");

	//wrefresh(ctrlWin);

	//wprintw(reminderWin, "hack uscs 2017 bdg");
	//wrefresh(reminderWin);
	outX = 1;
	outY = 1;

	while(1){
		if(fIsRunning){
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
				fIsRunning = 0;

				wgetch(getWin);
				wclear(getWin);
				goto skipBP;
			}
			ignoreBP = 0;
		}else{
			box(getWin,0,0);
			cIn = wgetch(getWin);
			//wrefresh(ctrlWin);

			switch(cIn){
				case 'q':
					return 0;
				case '\t':
					viewState++;
					if(viewState > 4)
						viewState = 1;
					changeView(viewState, primaryWin, secondaryWin);
					break;
				case 'r':
					fIsRunning=1;
					break;
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
				case KEY_LEFT:
					selectedWindow = 0;
					break;
				case KEY_RIGHT:
					selectedWindow = 1;
					break;
				case 'b':
					echo();
					mvwprintw(getWin,2,1, "Set breakpoint at: 0x");
					wscanw(getWin, "%x", &uBreakPoint);
					noecho();
					//wclear(getWin);
					wgetch(getWin);
					addBreakPoint(tMachine, uBreakPoint);
					wclear(getWin);
					break;
				default:
					break;
			}
		}

		printWindow(primaryWin, tMachine->szTape, tMachine->uTapeLen, tMachine);
		printWindow(secondaryWin, tMachine->szTape, tMachine->uTapeLen, tMachine);
	}

	endwin();
	return 0;
}