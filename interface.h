#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "curses.h"
#include "editor.h"
#include "debug.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "bf.h"

#define PRINT_TYPE_NUL 0x00
#define PRINT_TYPE_HEX 0x01
#define PRINT_TYPE_ASC 0x02
#define PRINT_TYPE_ASM 0x03

#define WINDOW_WIDTH_FULL 80
#define WINDOW_WIDTH_HALF 40
#define WINDOW_HEIGHT_FULL 21

#define HEXADDRLEN 5

struct WINDOW_T{
	WINDOW* tOutput;
	unsigned int uHeight, uWidth, uStartX, uStartY;
	
	char fIsVisible;
	char printType;

	unsigned int uAddressView;
};

struct WINDOW_T* addWindowView(int winHeight, int winWidth);

int changeWindowViewType(struct WINDOW_T* destWin, char printType);
int changeWindowViewAddr(struct WINDOW_T* destWin, unsigned int newAddr);
int toggleVisibility(struct WINDOW_T* destWin, char fIsVisible);
int toggleSize(unsigned int uHeight, unsigned int uWidth);

int printWindow(struct WINDOW_T* srcWin, char* szTape, unsigned int uTapeLen, struct MACHINE_STATE_T* tSrc);
int printWindowLine(WINDOW* destWin, char printType, int y, int x, char* szTape, unsigned int uAddress, struct MACHINE_STATE_T* tSrc);

#define VIEW_PRIMARY 1
#define VIEW_BOTH 2
#define VIEW_BOTH_FLIP 3
#define VIEW_SECONDARY 4
int changeView(int state, struct WINDOW_T* primaryWin, struct WINDOW_T* secondaryWin);

#endif

