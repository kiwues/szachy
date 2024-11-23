#pragma once

char getPieceFromBoard(char x, char y);
void setDefaultChessBoard();
void pickupPiece(char x, char y);
char* getBoardPtr();
void getLegalMoves(char xPick, char yPick, char* mobeBitMask,char* captureBitMask);