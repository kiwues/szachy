#include"boty.h"
#include"bot1.h"
Bot bots[1] = { 0 };


void Bot_Init() {
	bots[0].name = "Bot1";
	bots[0].MakeMove = Bot1_MakeMove;
}

void Bot_MakeMove(char botId) {
	char src = 0;
	char dest = 0;
	bots[botId-1].MakeMove(&src, &dest);
}