#include"board.h"

extern char gameStarted;
extern char botColor;//-1-player 0-white 1-black
extern ChessBoard displayBoard;
extern char depth;

void StartNewGame();
void NextRound();
void BotMove();