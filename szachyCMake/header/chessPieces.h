#pragma once

#define BLACK 8 //00001000
#define CHESSMASK 7 //00000111
#define KING 1//00000001
#define QUEEN 2//00000010
#define ROOK 3//00000011
#define BISHOP 4//00000100
#define KNIGHT 5//00000101
#define PAWN 6//00000110

void getMaskOfPiece(char piece, char x, char y, char* maskArray);