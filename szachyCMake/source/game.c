#include"../header/board.h"
#include"../header/interface.h"
#include"../header/input.h"

char gameStarted = 0;
char round = 0;
char check=0;
char pawnPromotion = 255;

void StartNewGame() {
	gameStarted = 1;
	setDefaultChessBoard();
	interface_resetRender();
	input_updateCursor();
}

void NextRound() {
	check = checkIfCheck(round ? 0 : 8) * (round + 1);
	char mateStale = checkIfCheckmateOrStalemateWhileChecked(round ? 0 : 8);
	if (mateStale) {
		if (mateStale == 1) {
			interface_showEnd(round ? 0 : 8);
		}
		else {
			interface_showEnd(-1);
		}
		gameStarted = 0;
	}
	round = !round;
}
