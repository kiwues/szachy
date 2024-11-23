#include<windows.h>

HANDLE hConsole;
HANDLE hSavedConsoleBuffer;

int window_init() {
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	hSavedConsoleBuffer = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	if (hSavedConsoleBuffer == INVALID_HANDLE_VALUE) {
		perror("Failed to save console buffer");
		return 1;
	}
}

void saveWindow() {
	SetConsoleActiveScreenBuffer(hSavedConsoleBuffer);
}

void restoreWindow() {
	SetConsoleActiveScreenBuffer(hConsole);
}

void window_deconstruct() {
	CloseHandle(hSavedConsoleBuffer);
}