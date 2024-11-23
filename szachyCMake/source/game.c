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
	round = !round;
}

void Check(char color) {
	check = color;
}
