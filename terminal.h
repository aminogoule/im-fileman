#pragma once
#include <string>
#include <termios.h>
#include "keycodes.h"
class Terminal {
public:
    Terminal();
    ~Terminal();

    void clearScreen();
    void moveCursor(int row, int col);
    void hideCursor();
    void showCursor();
    void flush();
    int readKey();
    void enableRawMode();
    void disableRawMode();

    int getWindowRows();
    int getWindowCols();

private:
    struct termios orig_termios;
};

