#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

void linuxTerminal_Init() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);

    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int getch() {
    int ch = 0;
    struct timeval tv;
    fd_set fds;

    read(STDIN_FILENO, &ch, 1);
    if (ch == 27) { 
        tv.tv_sec = 0
        tv.tv_usec = 100000; 
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);

        if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0) {
            int next1 = 0, next2 = 0;
            if (read(STDIN_FILENO, &next1, 1) == 1 && read(STDIN_FILENO, &next2, 1) == 1) {
                if (next1 == 91) { // '[' character
                    switch (next2) {
                        case 65: return 1000; // Up arrow
                        case 66: return 1001; // Down arrow
                        case 67: return 1002; // Right arrow
                        case 68: return 1003; // Left arrow
                    }
                }
            }
        } else {
            // escape key
            return 27;
        }
    }
    return ch;
}

void linuxTerminal_Deconstruct() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag |= ECHO | ICANON;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}