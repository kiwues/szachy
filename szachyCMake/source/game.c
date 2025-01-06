#include"../header/board.h"
#include"../header/interface.h"
#include"../header/input.h"
#include"../boty/boty.h"

char gameStarted = 0;
char botId = 0;
ChessBoard* mainBoard;

void StartNewGame() {
	gameStarted = 1;
	setDefaultChessBoard();
	mainBoard = getBoardPtr();
	interface_drawWholeBoard(mainBoard);
	input_updateCursor();
}

void BotMove() {
	if (botId && !!(botId & 128) == mainBoard->round) {
		Bot_MakeMove(botId & 127);
	}
}

void NextRound() {
	mainBoard->check = checkIfCheck(mainBoard->round ? 0 : 8, mainBoard) * (mainBoard->round + 1);
	char mateStale = checkIfCheckmateOrStalemateWhileChecked(mainBoard->round ? 0 : 8);
	if (mateStale) {
		if (mateStale == 1) {
			interface_showEnd(mainBoard->round ? 0 : 8);
		}
		else {
			interface_showEnd(-1);
		}
		gameStarted = 0;
	}
	mainBoard->round = !mainBoard->round;
	BotMove();
}
