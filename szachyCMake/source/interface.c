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

uint64_t drawingMoveBitmask =  0ull;
uint64_t drawingCaptureBitmask =0ull;

char isDrawingBitmask = 0;

char xDebug=0, yDebug=10;

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
		if (drawingMoveBitmask&(1ull<<(y*8+x))) {
			wprintf(L"\033[48;5;253m");
			return;
		}
		else if (drawingCaptureBitmask& (1ull << (y * 8 + x))) {
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
			if (displayBoard.pawnPromotion != -1&&(lastCursor_y < 0 || lastCursor_y>7)) {
				wprintf(L"\033[48;5;214m");
				printCorrectPiece(((displayBoard.pawnPromotion >> 4) + 3 - lastCursor_x)|(lastCursor_y==8?BLACK:0));
			}
			else {
				drawCorrectSquare(lastCursor_x, lastCursor_y);
				printCorrectPiece(getPieceFromBoard(lastCursor_x, lastCursor_y, &displayBoard));
			}
			moveCursorToBoard(x, y);
		}
		wprintf(L"\033[48;5;251m");
		if (displayBoard.pawnPromotion != -1&(y < 0 || y>7)) {
			printCorrectPiece(((displayBoard.pawnPromotion >> 4) + 3 - x) | (y == 8 ? BLACK : 0));
		}
		else {
			char piece = getPieceFromBoard(x, y,&displayBoard);
			printCorrectPiece(piece);
		}
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



void interface_drawWholeBoard(ChessBoard* board) {
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
			printCorrectPiece(getPieceFromBoard(x, y,board));
			wprintf(L"\033[0m");
		}
		wprintf(L"\033[48;5;42m");
		wprintf(L"\033[0m");
		wprintf(L"\n");
		wprintf(L"\033[0m");
	}
	input_updateCursor();
	lastCursor_x = -1;
	fflush(stdout);
}


void drawBitmask(uint64_t* ptrMoveBitmask, uint64_t* ptrCaptureBitmask) {
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (*ptrMoveBitmask& (1ull << (y * 8 + x))) {
				moveCursorToBoard(x, y);
				wprintf(L"\033[48;5;253m");
				printCorrectPiece(getPieceFromBoard(x, y, &displayBoard));
			}else if (*ptrCaptureBitmask&(1ull << (y * 8 + x))) {
				moveCursorToBoard(x, y);
				wprintf(L"\033[48;5;196m");
				printCorrectPiece(getPieceFromBoard(x, y,&displayBoard));
			}
		}
	}
	fflush(stdout);
}




void interface_showBitmask(uint64_t* ptrMoveBitmask, uint64_t* ptrCaptureBitmask) {
	drawingMoveBitmask = *ptrMoveBitmask;
	drawingCaptureBitmask = *ptrCaptureBitmask;
	drawBitmask(ptrMoveBitmask, ptrCaptureBitmask);
	input_updateCursor();
	isDrawingBitmask = 1;
}

void interface_clearBitmask() {
	if (!isDrawingBitmask) return;
	isDrawingBitmask = 0;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if ((drawingMoveBitmask&(1ull << (y * 8 + x)))|| (drawingCaptureBitmask&(1ull<<(y*8+x)))) {
				moveCursorToBoard(x, y);
				drawCorrectSquare(x, y);
				printCorrectPiece(getPieceFromBoard(x, y, &displayBoard));
			}
		}
	}
}

void interface_showLegalMovesOfCurrentPiece(char x, char y) {
	uint64_t bitMask = 0;
	uint64_t captureMask = 0ull;
	getLegalMoves(x, y,&bitMask,&captureMask,&displayBoard);
	//getMaskOfPiece(getPieceFromBoard(cursorPos >> 8, cursorPos & 255), cursorPos >> 8, cursorPos & 255, &bitMask);
	interface_showBitmask(&bitMask,&captureMask);
}


void interface_printMenu() {
	wprintf(L"1-Start new game vs player\n");
	wprintf(L"2-Start new game vs bot\n");
	wprintf(L"3-Find magic numbers\n");
	wprintf(L"ESC-Exit\n");
}

void interface_showEnd(GameState check) {
	moveCursorToBoard(0, 12);
	if(check<5)
		wprintf(L"Win for the %ls!", check==black_checkmate ? L"white" : L"black");
	else
		wprintf(L"Stalemate!");
}

void interface_showPawnPromotion(char x, char y) {
	displayBoard.pawnPromotion = (x << 4) | y;
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
	char* pawnPromotion = &displayBoard.pawnPromotion;
	if((*pawnPromotion &15)==0)
		moveCursorToBoard((*pawnPromotion >> 4)-2, (*pawnPromotion & 15)-1);
	else
		moveCursorToBoard((*pawnPromotion >> 4) - 2, (*pawnPromotion & 15)+1);
	wprintf(L"\033[0m  ");
	wprintf(L"\033[0m  ");
	wprintf(L"\033[0m  ");
	wprintf(L"\033[0m  ");
	lastCursor_x = -1;
	cursor_x = (*pawnPromotion >> 4);
	cursor_y = (*pawnPromotion & 15);
	input_updateCursor();
	*pawnPromotion = -1;
}

void interface_renderPiece(char x, char y) {
	moveCursorToBoard(x, y);
	drawCorrectSquare(x, y);
	printCorrectPiece(getPieceFromBoard(x,y, &displayBoard));
	input_updateCursor();
}
void interface_writeDebug(wchar_t* text) {
	moveCursorToBoard(xDebug, yDebug);
	/*xDebug += strlen(text);
	if (xDebug > 30)
	{
		xDebug = 12;
		yDebug++;
	}*/
	wprintf(L"%ls", text);
	input_updateCursor();
	fflush(stdout);
}