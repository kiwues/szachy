#include"../header/chessPieces.h"
#include"../header/board.h"
#include"../header/interface.h"
#include<stdio.h>
#include"../header/game.h"
#include<float.h>


#ifdef __linux__
#include<unistd.h>
#define SLEEP(ms) usleep((ms)*1000)
#else
#include<windows.h>
#define SLEEP(msg) Sleep(ms)
#endif

const unsigned short weights[6] = { 10000,1000,525,350,350,100 };
const float moveWeight = 0.01f;
const float captureWeight = 0.8f;
const float checkWeight = 0.1f;




float CalculateCaptureScore(char* board,char* captureMask) {
	float score = 0;
	for (char index = 0; index < 64; index++) {
		if (captureMask[index / 8] & (128 >> (index % 8))) {
			char piece = index % 2 ? (board[index / 2] >> 4) : (board[index / 2] & 15);
			if((piece&CHESSMASK))
				score += weights[(piece & CHESSMASK) - 1]* checkWeight;
			else
				score += weights[(piece & CHESSMASK) - 1];
		}
	}
	return score* captureWeight;
}

float Bot1_Evaluation(ChessBoard* board) {
	float wScore = 0;
	float bScore = 0;
	for (char index = 0; index < 64; index++)
	{
		char piece = getPieceFromBoardPtr(index%8,index/8,board);
		if (piece == 0)continue;
		char moveMask[8] = { 0 };
		char captureMask[8] = { 0 };
		if (piece & BLACK) {
			bScore += weights[(piece & CHESSMASK) - 1] +weights[(piece & CHESSMASK)-1]* getAmountOfLegalMovesOnBoardPtr(index%8,index/8,&moveMask,&captureMask,board)* moveWeight +CalculateCaptureScore(board,captureMask);
		}
		else {
			wScore += weights[(piece & CHESSMASK) - 1] +weights[(piece & CHESSMASK)-1] * getAmountOfLegalMovesOnBoardPtr(index % 8, index / 8, &moveMask, &captureMask,board)* moveWeight + CalculateCaptureScore(board, captureMask);
		}
	}
	return (wScore - bScore);
}

float Search(ChessBoard* board,char depth, char color,float alpha, float beta,char* pick, char* dest) {
	if (depth == 0) return (color&board->round==color ? -1 : 1) * Bot1_Evaluation(board);
	ChessBoard boardCopy;
	char notImportant = 0;
	memcpy(&boardCopy, board, sizeof(ChessBoard));
	for (char i = 0; i < 64; i++) {
		if (!!(getPieceFromBoardPtr(i % 8, i / 8, &boardCopy) & BLACK) == boardCopy.round) {
			char moveMask[8] = { 0 };
			char captureMask[8] = { 0 };
			getLegalMoves(i % 8, i / 8, &moveMask, &captureMask,&boardCopy);
			for (char j = 0; j < 64; j++)
			{
				if (!(captureMask[j / 8] & (128 >> (j % 8)))) continue;
				simulatePieceMoveOnBoardPtr(i % 8, i / 8, j % 8, j / 8, &boardCopy);
				boardCopy.round = !boardCopy.round;
				float eval = -Search(&boardCopy, depth - 1, color, -beta, -alpha, &notImportant, &notImportant);
				/*interface_drawWholeBoard(&boardCopy);
				wchar_t debugMessage[100];
				swprintf(debugMessage, sizeof(debugMessage) / sizeof(wchar_t), L"Eval: %f", eval);
				interface_writeDebug(debugMessage);
				SLEEP(1000);*/
				if (eval >= beta) {
					return beta;
				}
				if (alpha < eval) {
					*pick = (i % 8) << 4 | i / 8;
					*dest = (j % 8) << 4 | j / 8;
					alpha = eval;
				}
				memcpy(&boardCopy, board, sizeof(ChessBoard));
			}
			for (char j = 0; j < 64; j++)
			{
				if (!(moveMask[j / 8] & (128 >> (j % 8)))) continue;
				simulatePieceMoveOnBoardPtr(i % 8, i / 8, j % 8, j / 8, &boardCopy);
				boardCopy.round = !boardCopy.round;
				float eval = -Search(&boardCopy, depth - 1, color, -beta, -alpha, &notImportant, &notImportant);
				/*interface_drawWholeBoard(&boardCopy);
				wchar_t debugMessage[100];
				swprintf(debugMessage, sizeof(debugMessage) / sizeof(wchar_t), L"Eval: %f", eval);
				interface_writeDebug(debugMessage);
				SLEEP(1000);*/

				if (eval >= beta) {
					return beta;
				}
				if (alpha < eval) {
					*pick = (i % 8) << 4 | i / 8;
					*dest = (j % 8) << 4 | j / 8;
					alpha = eval;
				}
				memcpy(&boardCopy, board, sizeof(ChessBoard));
			}
		}
	}
	return alpha;
}

void Bot1_MakeMove(char* src, char* dest) {
	ChessBoard* board = getBoardPtr();
	float eval = Search(board, 3, !!(botId & 127), -FLT_MAX,FLT_MAX,src, dest);
	//interface_drawWholeBoard(board);

}
