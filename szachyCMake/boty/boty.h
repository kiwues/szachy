struct {
	const char* name;
	//pointer to function in corrensponding bot file
	void(*MakeMove)(char* src, char* dest);

}typedef Bot;

extern Bot bots[1];

void Bot_Init();
void Bot_MakeMove(char botId);