

extern char gameStarted;
extern char round;//0-white 1-black
extern char check; //1-white 2-black 0-none
extern char pawnPromotion;//x,y of promoting pawn
extern char botId;//0-player, first bit is color

void StartNewGame();
void NextRound();