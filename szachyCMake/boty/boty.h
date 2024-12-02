#include"../header/board.h"
struct {
	const char* name;
	//pointer to function in corrensponding bot file
	void(*MakeMove)(char* src, char* dest);
	float(*Evaluation)(ChessBoard* board);

}typedef Bot;

extern Bot bots[1];

void Bot_Init();
void Bot_MakeMove(char botId);