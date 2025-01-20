#include"board.h"

extern char gameStarted;
extern char botColor;//-1-player 0-white 1-black
extern ChessBoard displayBoard;

void StartNewGame();
void NextRound();
void BotMove();