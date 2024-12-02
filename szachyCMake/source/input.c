#include <stdio.h>
#include "../header/interface.h"
#include "../header/board.h"
#include "../header/game.h"

#ifndef __linux__
#include <conio.h>
#else
#include"../header/linuxTerminal.h"
#include <unistd.h>
#include <termios.h>
#endif // !__linux__

extern char running;

char cursor_y = 0;
char cursor_x = 0;

int offsetBoardx = 35;
int offsetBoardy = 2;



void moveCursorTo(char x, char y) {
    wprintf(L"\033[%d;%dH", y, x);
	fflush(stdout);
}

void moveCursorToBoard(char x, char y) {
    moveCursorTo(x * 2 + offsetBoardx, y + offsetBoardy);
}

void input_updateCursor() {
    moveCursorToBoard(cursor_x, cursor_y);
    drawCursor(cursor_x, cursor_y);
}

void input_init() {
#ifdef __linux__
    linuxTerminal_Init();
#endif // __linux__
}

void input_deconstruct() {
#ifdef __linux__
	linuxTerminal_Deconstruct();
#endif
}

short getCursorPosition() {
    return (cursor_x << 8) | cursor_y;
}

void gameControls(int ch) {
    ChessBoard* board=getBoardPtr();
    switch (ch) {
    case 32:
        //if (botId&&((botId & 128) >> 7) == round)break;
        pickupPiece(cursor_x, cursor_y);
        break;
    case 27:
        running = 0;
        break;
#ifdef __linux__
    case 1000:
        if (board->pawnPromotion != -1 && (board->pawnPromotion & 15) == 0 && cursor_y > -1 && ((board->pawnPromotion >> 4) - cursor_x<3 && (board->pawnPromotion >> 4) - cursor_x>-2)) {
            cursor_y--;
        }
        else {
            cursor_y--;
            if (cursor_y < 0) cursor_y++;
        }
        break;
    case 1001:
        if (board->pawnPromotion != -1 && (board->pawnPromotion & 15) == 8 && cursor_y < 8 && ((board->pawnPromotion >> 4) - cursor_x<3 && (board->pawnPromotion >> 4) - cursor_x>-2)) {
            cursor_y++;
        }
        else {
            cursor_y++;
            if (cursor_y > 7) cursor_y--;
        }
        break;
    case 1002:
        if (board->pawnPromotion != -1 && (cursor_y < 0 || cursor_y > 7)) {
            if ((board->pawnPromotion >> 4) - cursor_x > -1)
                cursor_x++;
        }
        else {
            cursor_x++;
            if (cursor_x > 7) cursor_x--;
        }
        break;
    case 1003:
        if (board->pawnPromotion != -1 && (cursor_y < 0 || cursor_y>7))
        {
            if (((board->pawnPromotion >> 4) - cursor_x < 2))
                cursor_x--;
        }
        else {
            cursor_x--;
            if (cursor_x < 0) cursor_x++;
        }
        break;
#else
    case 0:
    case 224:
    {
        ch = _getch();
        switch (ch) {
        case 72:
        {
            if (board->pawnPromotion != -1 && (board->pawnPromotion & 15) == 0 && cursor_y ==0 && ((board->pawnPromotion >> 4) - cursor_x<3 && (board->pawnPromotion >> 4) - cursor_x>-2)) {
                cursor_y--;
            }
            else {
                cursor_y--;
                if (cursor_y < 0) cursor_y++;
            }
            break;
        }
        case 80:
            if (board->pawnPromotion != -1 && (board->pawnPromotion & 15) == 7 && cursor_y == 7 && ((board->pawnPromotion >> 4) - cursor_x<3 && (board->pawnPromotion >> 4) - cursor_x>-2)) {
                cursor_y++;
            }
            else {
                cursor_y++;
                if (cursor_y > 7) cursor_y--;
            }
            break;
        case 75:
            if (board->pawnPromotion != -1&&(cursor_y<0||cursor_y>7))
            {
                if(((board->pawnPromotion >> 4) - cursor_x<2 ))
                    cursor_x--;
            }
            else {
                cursor_x--;
                if (cursor_x < 0) cursor_x++;
            }
            break;
        case 77:
            if (board->pawnPromotion != -1 && (cursor_y < 0 || cursor_y > 7)) {
                if ((board->pawnPromotion >> 4) - cursor_x > -1)
                    cursor_x++;
            }
            else {
                cursor_x++;
                if (cursor_x > 7) cursor_x--;
            }
            break;
        default:
            break;
        }
        break;
    }
#endif
    default:
        break;
    }
    input_updateCursor();
}


void input_loop() {

#ifdef __linux__
    int ch = getch();
#else
    int ch = _getch();
#endif
    if(gameStarted)
        gameControls(ch);
    else {
        switch (ch)
        {
        case '1':
            StartNewGame();
            break;
        case '2':
            StartNewGame();
            botId = 129;//color:black botid:1 
            BotMove();
            break;
        case 27:
            running = 0;
            break;
        default:
            break;
        }
    }
}
