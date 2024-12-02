#pragma once


typedef struct ChessBoard {
	char castling;// 0001-king didnt move, 0110-rooks didnt move start: 01110111 4bit-white 4bit-black
	char enPassant;//0-none or pawn position
	char check;//1-white 2-black 0-none
	char round;//0-white 1-black
	char pawnPromotion;//x,y of promoting pawn
	char board[32];
} ChessBoard;


char getPieceFromBoard(char x, char y);
char getPieceFromBoardPtr(char x, char y, ChessBoard* board);
void setDefaultChessBoard();
void pickupPiece(char x, char y);
void pickupAndPlacePiece(char xSrc, char ySrc,char xDst,char yDst);
void simulatePieceMoveOnBoardPtr(char xSrc, char ySrc, char xDst, char yDst, ChessBoard* board);
ChessBoard* getBoardPtr();
void getLegalMoves(char xPick, char yPick, char* moveBitMask, char* captureBitMask, ChessBoard* board);
char checkIfCheck(char color, ChessBoard* board);
char checkIfCheckmateOrStalemateWhileChecked(char color);
int getAmountOfLegalMoves(char x, char y, char* moveMask, char* captureMask);
int getAmountOfLegalMovesOnBoardPtr(char x, char y, char* moveMask, char* captureMask, ChessBoard* board);