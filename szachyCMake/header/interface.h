#pragma once
#include <wchar.h>
#include"../header/board.h"
#include<stdint.h>

//zmienic w vsc cursor width, minimum contrast ratio: 1, font size 

void interface_init();
void interface_deconstruct();
void interface_renderMovePiece(char sourceX, char sourceY, char destinationX, char destinationY, char piece);
void interface_renderPiece(char x, char y);
void drawCursor(char x,char y);
void interface_showBitmask(uint64_t* ptrMoveBitmask, uint64_t* ptrCaptureBitmask);
void interface_showLegalMovesOfCurrentPiece(char x, char y);
void interface_clearBitmask();
void interface_printMenu();
void interface_showEnd(GameState check);
void interface_showPawnPromotion(char x, char y);
void interface_clearPawnPromotion();
void interface_writeDebug(wchar_t* text);
void interface_drawWholeBoard(ChessBoard* board);