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
	checkIfCheck(!displayBoard.round, &displayBoard);
	if (displayBoard.check > 2) {
		interface_showEnd(displayBoard.check);
		gameStarted = 0;
		return;
	}
	displayBoard.round = !displayBoard.round;
	BotMove();
}
