#include"../header/board.h"
#include"../header/interface.h"
#include"../header/input.h"
#include"../bot.h"

char gameStarted = 0;
char botColor = -1;
ChessBoard displayBoard={0};

void StartNewGame() {
	InitBoard(&displayBoard);
	gameStarted = 1;
	interface_drawWholeBoard(&displayBoard);
	input_updateCursor();
}

void BotMove() {
	if (botColor == displayBoard.round) {
		Bot_MakeMove(botColor,&displayBoard);
	}
}

void NextRound() {
	displayBoard.check = checkIfCheck(!displayBoard.round, &displayBoard) * (displayBoard.round + 1);
	/*char mateStale = checkIfCheckmateOrStalemateWhileChecked(displayBoard.round ? 0 : 8, &displayBoard);
	if (mateStale) {
		if (mateStale == 1) {
			interface_showEnd(displayBoard.round ? 0 : 8);
		}
		else {
			interface_showEnd(-1);
		}
		gameStarted = 0;
	}*/
	displayBoard.round = !displayBoard.round;
	BotMove();
}
