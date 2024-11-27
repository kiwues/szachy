#pragma once


extern char cursor_y;
extern char cursor_x;

void input_init();
void input_deconstruct();
void input_loop();
void moveCursorToBoard(char x, char y);
//first 8bits->x rest->y
short getCursorPosition();
void input_updateCursor();