#include"header/chessPieces.h"
#include"header/board.h"
#include"header/interface.h"
#include<stdio.h>
#include"header/game.h"
#include<float.h>


#ifdef __linux__
#include<unistd.h>
#define SLEEP(ms) usleep((ms)*1000)
#else
#include<windows.h>
#define SLEEP(msg) Sleep(ms)
#endif

const unsigned int weights[6] = { 10000,1000,525,350,350,100 };
const float moveWeight = 0.01f;
const float captureWeight = 0.8f;
const float checkWeight = 0.1f;




float CalculateCaptureScore(ChessBoard* board, char color,uint64_t* captureMask) {
	float score = 0;
	Bitmaps *mask = color ? &board->whiteBitmaps:&board->blackBitmaps;
	score += MaskBitCount(*captureMask & mask->king) * weights[0];
	score += MaskBitCount(*captureMask & mask->queen) * weights[1];
	score += MaskBitCount(*captureMask & mask->rook) * weights[2];
	score += MaskBitCount(*captureMask & mask->bishop) * weights[3];
	score += MaskBitCount(*captureMask & mask->knight) * weights[4];
	score += MaskBitCount(*captureMask & mask->pawn) * weights[5];
	/*for (char index = 0; index < 64; index++) {
		if (captureMask[index / 8] & (128 >> (index % 8))) {
			char piece = index % 2 ? (board[index / 2] >> 4) : (board[index / 2] & 15);
			if ((piece & CHESSMASK))
				score += weights[((piece & CHESSMASK) - 1)] * checkWeight;
			else
				score += weights[((piece & CHESSMASK) - 1)];
		}
	}*/
	return score * captureWeight;
}

float Bot_Evaluation(ChessBoard* board) {
	float wScore = 0;
	float bScore = 0;
	for (char index = 0; index < 64; index++)
	{
		char piece = getPieceFromBoard(index % 8, index / 8, board);
		if (piece == 0)continue;
		uint64_t moveMask = 0ull;
		uint64_t captureMask = 0ull;
		getLegalMoves(index % 8, index / 8, &moveMask, &captureMask, board);
		if (piece & BLACK) {
			bScore += weights[((piece & CHESSMASK) - 1)] + weights[((piece & CHESSMASK) - 1)] * MaskBitCount(moveMask|captureMask) * moveWeight + CalculateCaptureScore(board,BLACK, &captureMask);
		}
		else {
			wScore += weights[((piece & CHESSMASK) - 1)] + weights[((piece & CHESSMASK) - 1)] * MaskBitCount(moveMask | captureMask) * moveWeight + CalculateCaptureScore(board, 0,&captureMask);
		}
	}
	return (wScore - bScore);
}

float Search(ChessBoard* board, char depth, char color, float alpha, float beta, char* pick, char* dest) {
	if (depth == 0) return (color & board->round == color ? -1 : 1) * Bot_Evaluation(board);
	ChessBoard boardCopy;
	char notImportant = 0;
	memcpy(&boardCopy, board, sizeof(ChessBoard));
	for (char i = 0; i < 64; i++) {
		if (!!(board->board[i] & BLACK) == boardCopy.round) {
			uint64_t moveMask = 0ull;
			uint64_t captureMask = 0ull;
			getLegalMoves(i % 8, i / 8, &moveMask, &captureMask, &boardCopy);
			while (captureMask) {
				int moveIndex = getIndexOfFirstBit(&captureMask);
				captureMask &= (captureMask - 1);

				simulatePieceMoveOnBoard(i % 8, i / 8, moveIndex % 8, moveIndex / 8, &boardCopy);
				boardCopy.round = !boardCopy.round;
				checkIfCheck(boardCopy.round, &boardCopy);
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
					*pick = i;
					*dest = moveIndex;
					alpha = eval;
				}
				memcpy(&boardCopy, board, sizeof(ChessBoard));

			}

			while (moveMask) {
				int moveIndex = getIndexOfFirstBit(&moveMask);
				moveMask &= (moveMask - 1);

				simulatePieceMoveOnBoard(i % 8, i / 8, moveIndex % 8, moveIndex / 8, &boardCopy);
				boardCopy.round = !boardCopy.round;
				checkIfCheck(boardCopy.round, &boardCopy);
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
					*pick = i;
					*dest = moveIndex;
					alpha = eval;
				}
				memcpy(&boardCopy, board, sizeof(ChessBoard));
			}
		}
	}
	return alpha;
}

void Bot_MakeMove(char botColor, ChessBoard* board) {
	char src = 0;
	char dest = 0;
	float eval = Search(board, 3, botColor, -FLT_MAX, FLT_MAX, &src, &dest);
	//interface_drawWholeBoard(board);
	pickupAndPlacePiece(src%8, src /8, dest %8, dest /8, board);
	//float eval = Evaluation(board);
	wchar_t debugMessage[50];
	swprintf(debugMessage, sizeof(debugMessage) / sizeof(wchar_t), L"Eval: %f\n move: (%d;%d), (%d;%d)", eval, src%8, src /8, dest %8, dest /8);
	interface_writeDebug(debugMessage);

}
