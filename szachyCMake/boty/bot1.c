#include"../header/chessPieces.h"
#include"../header/board.h"
#include"../header/interface.h"
#include<stdio.h>

const unsigned short weights[6] = { 10000,1000,525,350,350,100 };
const float moveWeight = 0.1f;
const float captureWeight = 0.8f;



float CalculateCaptureScore(char* board,char* captureMask) {
	float score = 0;
	for (char index = 0; index < 64; index++) {
		if (captureMask[index / 8] & (128 >> (index % 8))) {
			char piece = index % 2 ? (board[index / 8] >> 4) : (board[index / 2] & 15);
			score += weights[(piece & CHESSMASK) - 1];
		}
	}
	return score* captureWeight;
}

float Bot1_Evaluation(char* board) {
	float wScore = 0;
	float bScore = 0;
	for (char index = 0; index < 64; index++)
	{
		char piece = (board[index/2]>>(4*!(index%2)))&15;
		char piece2 = getPieceFromBoard(index / 8, index % 8);
		if (piece != piece2) {
			wprintf(L"Error in board evaluation\n");
		}
		if (piece == 0)continue;
		char moveMask[8] = { 0 };
		char captureMask[8] = { 0 };
		if (piece & BLACK) {
			bScore += weights[(piece & CHESSMASK) - 1] +weights[(piece & CHESSMASK)-1]*getAmountOfLegalMoves(index/8,index%8,&moveMask,&captureMask)* moveWeight +CalculateCaptureScore(board,captureMask);
		}
		else {
			wScore += weights[(piece & CHESSMASK) - 1] +weights[(piece & CHESSMASK)-1] * getAmountOfLegalMoves(index / 8, index % 8, &moveMask, &captureMask)* moveWeight + CalculateCaptureScore(board, captureMask);
		}
	}
	return (wScore - bScore);
}

void Bot1_MakeMove(char* src, char* dest) {
	float eval = Bot1_Evaluation(getBoardPtr());
    wchar_t debugMessage[50];
    swprintf(debugMessage, sizeof(debugMessage) / sizeof(wchar_t), L"Eval: %f", eval);
    interface_writeDebug(debugMessage);
}
