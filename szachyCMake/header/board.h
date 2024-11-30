#pragma once

char getPieceFromBoard(char x, char y);
void setDefaultChessBoard();
void pickupPiece(char x, char y);
char* getBoardPtr();
void getLegalMoves(char xPick, char yPick, char* mobeBitMask,char* captureBitMask);
char checkIfCheck(char color);
char checkIfCheckmateOrStalemateWhileChecked(char color);
int getAmountOfLegalMoves(char x, char y, char* moveMask, char* captureMask);