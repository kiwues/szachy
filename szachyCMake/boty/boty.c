#include"boty.h"
#include"bot1.h"
#include"../header/board.h"
#include"../header/interface.h"
Bot bots[1] = { 0 };


void Bot_Init() {
	bots[0].name = "Bot1";
	bots[0].MakeMove = Bot1_MakeMove;
	bots[0].Evaluation = Bot1_Evaluation;
}

void Bot_MakeMove(char botId) {
	char src = 0;
	char dest = 0;
	bots[botId-1].MakeMove(&src, &dest);
	pickupAndPlacePiece(src >>4, src &15, dest >>4, dest &15);
	float eval = bots[botId - 1].Evaluation(getBoardPtr());
	wchar_t debugMessage[50];
	swprintf(debugMessage, sizeof(debugMessage) / sizeof(wchar_t), L"Eval: %f\n move: (%d;%d), (%d;%d)", eval, src>>4, src&15, dest>>4, dest&15);
	interface_writeDebug(debugMessage);
}