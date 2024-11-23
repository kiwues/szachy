#include"../header/interface.h"
#include"../header/board.h"
#include"../header/input.h"
#include<stdbool.h>

char running = 0;

void app_loop() {
	while (running) {
		input_loop();
	}
}

void app_init() {
	interface_init();
	setDefaultChessBoard();
	input_init();
	interface_printMenu();
	running = 1;
}

void app_deconstruct() {
	input_deconstruct();
	interface_deconstruct();
}