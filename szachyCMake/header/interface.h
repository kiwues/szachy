#pragma once

//zmienic w vsc cursor width, minimum contrast ratio 1, font size 

void interface_init();
void interface_resetRender();
void interface_deconstruct();
void interface_renderMovePiece(char sourceX, char sourceY, char destinationX, char destinationY, char piece);
void drawCursor(char x,char y);
void interface_showBitmask(char* ptrBitmask);
void interface_showLegalMovesOfCurrentPiece();
void interface_clearBitmask();
void interface_printMenu();