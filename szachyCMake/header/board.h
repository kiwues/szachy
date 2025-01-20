#pragma once
#include<stdint.h>

typedef enum{
	normal,
	white_check,
	black_check,
	white_checkmate,
	black_checkmate,
	white_stalemate,
	black_stalemate
}GameState;

typedef struct Bitmaps {
	uint64_t king;
	uint64_t queen;
	uint64_t rook;
	uint64_t bishop;
	uint64_t knight;
	uint64_t pawn;
	uint64_t all;
	uint64_t lockedPieces;
	uint64_t lockedPieces_move;
	uint64_t squaresToDefend;
}Bitmaps;

typedef struct ChessBoard {
	char castling;// 0001-king didnt move, 0110-rooks didnt move start: 01110111 first 4bit-white 4bit-black
	char enPassant;//0-none or square of pawn
	GameState check;
	char round;//0-white 1-black
	char pawnPromotion;//x,y of promoting pawn
	char board[64];
	Bitmaps whiteBitmaps;
	Bitmaps blackBitmaps;
	uint64_t allBitmap; 
} ChessBoard;



char getPieceFromBoard(char x, char y, ChessBoard* board);
void InitBoard(ChessBoard* board);
void pickupPiece(char x, char y, ChessBoard* board);
void pickupAndPlacePiece(char xSrc, char ySrc,char xDst,char yDst, ChessBoard* board);
void simulatePieceMoveOnBoard(char xSrc, char ySrc, char xDst, char yDst, ChessBoard* board);
void getLegalMoves(char xPick, char yPick, uint64_t* moveBitMask, uint64_t* captureBitMask, ChessBoard* board);
GameState checkIfCheck(char color, ChessBoard* board);
char checkIfCheckmateOrStalemateWhileChecked(char color, ChessBoard* board);
//int getAmountOfLegalMoves(char x, char y, char* moveMask, char* captureMask);
//int getAmountOfLegalMovesOnBoard(char x, char y, char* moveMask, char* captureMask, ChessBoard* board);
void setBitmaps(ChessBoard* board);
int MaskBitCount(uint64_t mask);
int getIndexOfFirstBit(uint64_t* bitmap);