#include <stdio.h>
#include<wchar.h>
#include <locale.h>
#include"../header/board.h"
#include<stdlib.h>
#include"../header/input.h"
#include"../header/chessPieces.h"
#include"../header/game.h"


#ifndef __linux__
#include"../header/window.h"
void saveTerminal() {
	saveWindow();
}

void restoreTerminal() {
	restoreWindow();
}
#else
void saveTerminal() {
	system("tput smcup");
}

void restoreTerminal() {
	system("tput rmcup");
}
#endif


const wchar_t* chess_pieces_characters[13] = {L"0m ",L"255m♚", L"255m♛", L"255m♜", L"255m♝", L"255m♞", L"255m♟", L"0m♚", L"0m♛", L"0m♜", L"0m♝", L"0m♞", L"0m♟"};
//const wchar_t* chess_pieces_characters[13] = {L"0m ", L"0m♚", L"0m♛", L"0m♜", L"0m♝", L"0m♞", L"0m♟",L"255m♚", L"255m♛", L"255m♜", L"255m♝", L"255m♞", L"255m♟"};
//const wchar_t* chess_pieces_characters[13] = {L" ",L"♔", L"♕", L"♖", L"♗", L"♘", L"♙", L"♚", L"♛", L"♜", L"♝", L"♞", L"♟"};

char update = 1;

char lastCursor_x=-1;
char lastCursor_y=-1;

char drawingMoveBitmask[8] = { 0 };
char drawingCaptureBitmask[8] = { 0 };

char isDrawingBitmask = 0;

void interface_init() {
#ifndef __linux__
	_setmode(_fileno(stdout), 0x00020000);
	window_init();
#else
	setlocale(LC_ALL, "");
	//system("/bin/stty raw -echo");
#endif 
	saveTerminal();

}

void interface_deconstruct() {
	restoreTerminal();
#ifndef __linux__
	window_deconstruct();
#else
	//system("/bin/stty cooked echo");
#endif 
}

void printCorrectPiece(char piece) {
	wprintf(L"\033[38;5;%ls ", chess_pieces_characters[6 * (piece >> 3) + (piece & 7)]);
}

void drawCorrectSquare(char x, char y)
{
	if (isDrawingBitmask)
	{
		if (drawingMoveBitmask[y] & (128 >> x)) {
			wprintf(L"\033[48;5;253m");
			return;
		}
		else if (drawingCaptureBitmask[y] & (128 >> x)) {
			wprintf(L"\033[48;5;196m");
			return;
		}
	}
	if ((y + x) % 2) {
		wprintf(L"\033[48;5;130m");
	}
	else {
		wprintf(L"\033[48;5;214m");
	}
}

void drawCursor(char x, char y) {
	if(x!=lastCursor_x||y!=lastCursor_y)
	{
		if (lastCursor_x != -1) {
			moveCursorToBoard(lastCursor_x, lastCursor_y);
			if (pawnPromotion != -1&&(lastCursor_y < 0 || lastCursor_y>7)) {
				wprintf(L"\033[48;5;214m");
				printCorrectPiece(((pawnPromotion >> 4) + 3 - lastCursor_x)|(lastCursor_y==8?BLACK:0));
			}
			else {
				drawCorrectSquare(lastCursor_x, lastCursor_y);
				printCorrectPiece(getPieceFromBoard(lastCursor_x, lastCursor_y));
			}
			moveCursorToBoard(x, y);
		}
		wprintf(L"\033[48;5;251m");
		if (pawnPromotion != -1&(y < 0 || y>7)) {
			printCorrectPiece(((pawnPromotion >> 4) + 3 - x) | (y == 8 ? BLACK : 0));
		}else
			printCorrectPiece(getPieceFromBoard(x, y));
		moveCursorToBoard(x, y);
		lastCursor_x = x;
		lastCursor_y = y;
		fflush(stdout);
	}
}

void interface_renderMovePiece(char sourceX,char sourceY,char destinationX,char destinationY,char piece) {
	moveCursorToBoard(sourceX, sourceY);
	if ((sourceY + sourceX) % 2) {
		wprintf(L"\033[48;5;130m");
	}
	else {
		wprintf(L"\033[48;5;214m");
	}
	printCorrectPiece(0);
	moveCursorToBoard(destinationX, destinationY);
	printCorrectPiece(piece);
	lastCursor_x = -1;
	input_updateCursor();
	fflush(stdout);
}



void drawWholeBoard() {
#ifdef __linux__
	system("clear");
#else 	
	system("cls");
#endif // __linux__

	for (char y = 0; y < 8; y++)
	{
		moveCursorToBoard(0, y);
		for (char x = 0; x < 8; x++)
		{
			drawCorrectSquare(x, y);
			printCorrectPiece(getPieceFromBoard(x, y));
		}
		wprintf(L"\033[48;5;42m");
		wprintf(L"\n");
		wprintf(L"\033[0m");
	}
	input_updateCursor();
	lastCursor_x = -1;
	fflush(stdout);
}


void drawBitmask(char* ptrMoveBitmask, char* ptrCaptureBitmask) {
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if ((ptrMoveBitmask[y] >> (7 - x)) & 1) {
				moveCursorToBoard(x, y);
				wprintf(L"\033[48;5;253m");
				printCorrectPiece(getPieceFromBoard(x, y));
			}else if ((ptrCaptureBitmask[y] >> (7 - x)) & 1) {
				moveCursorToBoard(x, y);
				wprintf(L"\033[48;5;196m");
				printCorrectPiece(getPieceFromBoard(x, y));
			}
		}
	}
	fflush(stdout);
}


void interface_resetRender() {
	drawWholeBoard();
}



void interface_showBitmask(char* ptrMoveBitmask, char* ptrCaptureBitmask) {
	memcpy(&drawingMoveBitmask, ptrMoveBitmask, 8);
	memcpy(&drawingCaptureBitmask, ptrCaptureBitmask, 8);
	drawBitmask(ptrMoveBitmask, ptrCaptureBitmask);
	input_updateCursor();
	isDrawingBitmask = 1;
}

void interface_clearBitmask() {
	if (!isDrawingBitmask) return;
	isDrawingBitmask = 0;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (((drawingMoveBitmask[y] >> (7 - x)) & 1)|| ((drawingCaptureBitmask[y] >> (7 - x)) & 1)) {
				moveCursorToBoard(x, y);
				drawCorrectSquare(x, y);
				printCorrectPiece(getPieceFromBoard(x, y));
			}
		}
	}
}

void interface_showLegalMovesOfCurrentPiece() {
	short cursorPos = getCursorPosition();
	char* bitMask[8] = { 0 };
	char* captureMask[8] = { 0 };
	getLegalMoves(cursorPos >> 8, cursorPos & 255,&bitMask,&captureMask);
	//getMaskOfPiece(getPieceFromBoard(cursorPos >> 8, cursorPos & 255), cursorPos >> 8, cursorPos & 255, &bitMask);
	interface_showBitmask(bitMask,captureMask);
}


void interface_printMenu() {
	wprintf(L"1-Start new game\n");
	wprintf(L"ESC-Exit\n");
}

void interface_showEnd(char color) {
	moveCursorToBoard(0, 12);
	if(color>=0)
		wprintf(L"Win for the %ls!", color ? L"white" : L"black");
	else 
		wprintf(L"Stalemate!");
}

void interface_showPawnPromotion(char x, char y) {
	pawnPromotion = (x << 4) | y;
	char addition = 0;
	if (y == 0) {
		moveCursorToBoard(x - 2, y - 1);
	}
	else {
		moveCursorToBoard(x - 2, y + 1);
		addition = 1 << 3;
	}
	printCorrectPiece(KNIGHT|addition);
	printCorrectPiece(BISHOP | addition);
	printCorrectPiece(ROOK | addition);
	printCorrectPiece(QUEEN | addition);
	moveCursorToBoard(x, y);
	fflush(stdout);
}

void interface_clearPawnPromotion() {
	if((pawnPromotion&15)==0)
		moveCursorToBoard((pawnPromotion >> 4)-2, (pawnPromotion & 15)-1);
	else
		moveCursorToBoard((pawnPromotion >> 4) - 2, (pawnPromotion & 15)+1);
	wprintf(L"\033[0m  ");
	wprintf(L"\033[0m  ");
	wprintf(L"\033[0m  ");
	wprintf(L"\033[0m  ");
	lastCursor_x = -1;
	cursor_x = (pawnPromotion >> 4);
	cursor_y = (pawnPromotion & 15);
	input_updateCursor();
	pawnPromotion = -1;
}

void interface_renderPiece(char x, char y) {
	moveCursorToBoard(x, y);
	drawCorrectSquare(x, y);
	printCorrectPiece(getPieceFromBoard(x,y));
	input_updateCursor();
}
