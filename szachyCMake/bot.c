#include"header/chessPieces.h"
#include"header/board.h"
#include"header/interface.h"
#include<stdio.h>
#include"header/game.h"
#include<float.h>
#include <time.h>


#ifdef __linux__
#include<unistd.h>
#define SLEEP(ms) usleep((ms)*1000)
#else
#include<windows.h>
#define SLEEP(msg) Sleep(ms)
#endif

const unsigned int weights[6] = { 10000,1000,525,350,350,100 };
//const float moveWeight = 0.01f;
//const float captureWeight = 0.8f;
//const float checkWeight = 0.1f;




/*float CalculateCaptureScore(ChessBoard* board, char color, uint64_t* captureMask) {
	float score = 0;
	Bitmaps *mask = color ? &board->whiteBitmaps:&board->blackBitmaps;
	score += MaskBitCount(*captureMask & mask->king) * weights[0];
	score += MaskBitCount(*captureMask & mask->queen) * weights[1];
	score += MaskBitCount(*captureMask & mask->rook) * weights[2];
	score += MaskBitCount(*captureMask & mask->bishop) * weights[3];
	score += MaskBitCount(*captureMask & mask->knight) * weights[4];
	score += MaskBitCount(*captureMask & mask->pawn) * weights[5];
	return score * captureWeight;
}*/

float Bot_Evaluation(ChessBoard* board) {
	float wScore = 0;
	float bScore = 0;
	for (char index = 0; index < 64; index++)
	{
		char piece = getPieceFromBoard(index % 8, index / 8, board);
		if (piece == 0)continue;
		/*uint64_t moveMask = 0ull;
		uint64_t captureMask = 0ull;
		getLegalMoves(index % 8, index / 8, &moveMask, &captureMask, board);*/
		if (piece & BLACK) {
			bScore += weights[((piece & CHESSMASK) - 1)]; //+ weights[((piece & CHESSMASK) - 1)] * MaskBitCount(moveMask | captureMask) * moveWeight + CalculateCaptureScore(board,BLACK, &captureMask);
		}
		else {
			wScore += weights[((piece & CHESSMASK) - 1)]; //+ weights[((piece & CHESSMASK) - 1)] //* MaskBitCount(moveMask | captureMask) * moveWeight + CalculateCaptureScore(board, 0,&captureMask);
		}
	}
	if (board->check == black_checkmate) 			return (board->round ? -1 : 1) * FLT_MAX;
	else if (board->check == white_checkmate)			return (board->round ? -1 : 1) * -FLT_MAX;
	else if (board->check > 4) return 0;
	else return (board->round?-1:1)*(wScore - bScore);
}

float Search(ChessBoard* board, char depth, float alpha, float beta, Move* bestMove) {
	if (depth == 0||board->check>2) {
		return Bot_Evaluation(board);//(color & board->round == color ? -1 : 1) * 
	}
	ChessBoard boardCopy;
	Move notImportant;
	element_listy* move = GetAllMovesFor(board->round, board);
	if (move == NULL)return Bot_Evaluation(board);
	element_listy* moves_head = move;
	do {
		boardCopy = *board;
		//pawn promotion
		simulatePieceMoveOnBoard(&move->move, &boardCopy);
		if ((boardCopy.round &&move->move.yTo == 7 && (CHESSMASK & getPieceFromBoard(move->move.xTo, move->move.yTo, &boardCopy)) == PAWN)||
			(!boardCopy.round && move->move.yTo == 0 && (CHESSMASK & getPieceFromBoard(move->move.xTo, move->move.yTo, &boardCopy)) == PAWN)) {
			for (int i = 0; i < 4; i++) {
				PromotePawn(move->move.xTo, move->move.yTo, (QUEEN+i) | (boardCopy.round ?BLACK:0), &boardCopy);
				boardCopy.round = !boardCopy.round;
				checkIfCheck(boardCopy.round, &boardCopy);
				float eval = -Search(&boardCopy, depth - 1, -beta, -alpha, &notImportant);
				if (eval >= beta) {
					lista_zwolnij(move);
					return beta;
				}
				if (alpha < eval) {
					*bestMove = move->move;
					bestMove->promotionPiece = QUEEN + i;
					alpha = eval;
				}
				boardCopy = *board;
			}
			continue;
		}
		boardCopy.round = !boardCopy.round;
		checkIfCheck(boardCopy.round, &boardCopy);
		float eval = -Search(&boardCopy, depth - 1, -beta, -alpha, &notImportant);

		/*interface_drawWholeBoard(&boardCopy);
		wchar_t debugMessage[100];
		swprintf(debugMessage, sizeof(debugMessage) / sizeof(wchar_t), L"Eval: %f", eval);
		interface_writeDebug(debugMessage);
		SLEEP(1000);*/
		if (eval >= beta) {
			lista_zwolnij(move);
			return beta;
		}
		if (alpha < eval) {
			*bestMove = move->move;
			alpha = eval;
		}
	} while (move->nastepny != NULL && (move = lista_usun_ity(move, 0)));//(move = lista_usun_ity(move,0)) (move = move->nastepny)
	//lista_zwolnij(moves_head);
	lista_zwolnij(move);
	return alpha;
}



void Bot_MakeMove(char botColor, int depth,ChessBoard* board) {
    Move bestMove;
    clock_t start, end;
    int cpu_time_used;

    start = clock();
    float eval = Search(board, depth, -FLT_MAX, FLT_MAX, &bestMove);
    end = clock();

    cpu_time_used = ((int) (end - start));

	simulatePieceMoveOnBoard(&bestMove, board);
	NextRound();
	interface_renderPiece(bestMove.xFrom, bestMove.yFrom);
	interface_renderPiece(bestMove.xTo, bestMove.yTo);

    wchar_t debugMessage[100];
	if (eval == -FLT_MAX || eval == FLT_MAX) {
		swprintf(debugMessage, sizeof(debugMessage) / sizeof(wchar_t), L"Ocena: mat ruch: (%c%d) do (%c%d), glebokosc: %d, czas: %d ms", 'A' + (7 - bestMove.xFrom), bestMove.yFrom + 1, 'A' + (7 - bestMove.xTo), bestMove.yTo + 1, depth, cpu_time_used);
	}else
		swprintf(debugMessage, sizeof(debugMessage) / sizeof(wchar_t), L"Ocena: %f ruch: (%c%d) do (%c%d), glebokosc: %d, czas: %d ms", eval, 'A' + (7 - bestMove.xFrom) , bestMove.yFrom+1,  'A'+(7-bestMove.xTo), bestMove.yTo + 1, depth, cpu_time_used);
    interface_writeDebug(debugMessage);
}
