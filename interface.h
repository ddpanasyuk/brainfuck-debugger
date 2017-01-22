#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "curses.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "bf.h"

//different forms of portraying data and program text
#define PRINT_TYPE_NUL 0x00
#define PRINT_TYPE_HEX 0x01
#define PRINT_TYPE_ASC 0x02
#define PRINT_TYPE_ASM 0x03

//window width/height presets
#define WINDOW_WIDTH_FULL 80
#define WINDOW_WIDTH_HALF 40
#define WINDOW_HEIGHT_FULL 21

//definitions for various window view formats
#define VIEW_PRIMARY 1
#define VIEW_BOTH 2
#define VIEW_BOTH_FLIP 3
#define VIEW_SECONDARY 4

//length of a hex address
#define HEXADDRLEN 5

//slightly expanded window structure 
struct WINDOW_T{
	//base pdcurses WINDOW structure
	WINDOW* tOutput;

	//window location attributes
	unsigned int uHeight, uWidth, uStartX, uStartY;
	
	//window visiblity and view attributes
	char fIsVisible;
	char printType;

	//tape address windows begins displaying from
	unsigned int uAddressView;
};

//add a window view to machine tape
struct WINDOW_T* addWindowView(int winHeight, int winWidth);

//switches the output format of selected window
int changeWindowViewType(struct WINDOW_T* destWin, char printType);
//changes tape address that window is directed at
int changeWindowViewAddr(struct WINDOW_T* destWin, unsigned int newAddr);
//toggles visibility of window
int toggleVisibility(struct WINDOW_T* destWin, char fIsVisible);
//toggles window size
int toggleSize(unsigned int uHeight, unsigned int uWidth);

//prints out tape from machine using selected window
int printWindow(struct WINDOW_T* srcWin, char* szTape, unsigned int uTapeLen, struct MACHINE_STATE_T* tSrc);
//prints out one line of selected window (used as helper function for printWindow())
int printWindowLine(WINDOW* destWin, char printType, int y, int x, char* szTape, unsigned int uAddress, struct MACHINE_STATE_T* tSrc);

//quickly change window display locations / sizes using preset views
int changeView(int state, struct WINDOW_T* primaryWin, struct WINDOW_T* secondaryWin);

#endif

