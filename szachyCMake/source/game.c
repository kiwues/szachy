#include"../header/board.h"
#include"../header/interface.h"
#include"../header/input.h"

char gameStarted = 0;
char round = 0;
char check=0;

void StartNewGame() {
	gameStarted = 1;
	setDefaultChessBoard();
	interface_resetRender();
	input_updateCursor();
}

void NextRound() {
	if (checkIfCheck(round ? 0 : 8))
	{
		check = round + 1;
		if (checkIfCheckmateOrStalemateWhileChecked(round ? 0 : 8)) {
			interface_showWin(round ? 0 : 8);
			gameStarted = 0;
		}
	}
	round = !round;
}
