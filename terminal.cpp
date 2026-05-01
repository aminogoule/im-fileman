#include "terminal.h"
#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

Terminal::Terminal() {
    enableRawMode();
    hideCursor();
    clearScreen();
}

Terminal::~Terminal() {
    disableRawMode();
    showCursor();
}

void Terminal::enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    raw.c_iflag &= ~(IXON | ICRNL);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void Terminal::disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void Terminal::clearScreen() {
    std::cout << "\033[2J\033[H";
}

void Terminal::moveCursor(int row, int col) {
    std::cout << "\033[" << row << ";" << col << "H";
}

void Terminal::hideCursor() {
    std::cout << "\033[?25l";
}

void Terminal::showCursor() {
    std::cout << "\033[?25h";
}

void Terminal::flush() {
    std::cout << std::flush;
}

int Terminal::readKey() {
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1) return -1;

    if (c != '\x1b') {
        return c; // обычный символ
    }

    char seq[6];
    if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
    if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';

    if (seq[0] == '[') {
        if (seq[1] >= 'A' && seq[1] <= 'D') {
            // стрелки
            switch (seq[1]) {
                case 'A': return KEY_UP;
                case 'B': return KEY_DOWN;
                case 'C': return KEY_RIGHT;
                case 'D': return KEY_LEFT;
            }
        }

        // читаем оставшиеся символы для F-клавиш
        char seq2[3] = {};
        read(STDIN_FILENO, &seq2[0], 1);
        if (seq2[0] == '~') {
            switch (seq[1]) {
                case '1': return KEY_HOME;
                case '2': return KEY_INSERT;
                case '3': return KEY_DELETE;
                case '4': return KEY_END;
                case '5': return KEY_PAGEUP;
                case '6': return KEY_PAGEDOWN;
            }
        }

        if (seq[1] == '1' || seq[1] == '2') {
            read(STDIN_FILENO, &seq2[1], 1); // ожидаем ; или ~

            if (seq2[1] == '5') { // ;5R, Alt, Ctrl и т.п. — можно расширять
                read(STDIN_FILENO, &seq2[2], 1);
            } else if (seq2[1] == '~') {
                if (seq[0] == '1') return KEY_HOME;
            }
        }

        // F1–F4 — короткий ESC [OP формат
        if (seq[0] == 'O') {
            switch (seq[1]) {
                case 'P': return KEY_F(1);
                case 'Q': return KEY_F(2);
                case 'R': return KEY_F(3);
                case 'S': return KEY_F(4);
            }
        }

        // F5–F12 — ESC [15~ … ESC [24~
        if (seq[0] == '[') {
            switch (seq[1]) {
                case '1': {
                    read(STDIN_FILENO, &seq2[0], 1);
                    if (seq2[0] == '5') {
                        read(STDIN_FILENO, &seq2[1], 1);
                        if (seq2[1] == '~') return KEY_F(5);
                    }
                    if (seq2[0] == '7') {
                        read(STDIN_FILENO, &seq2[1], 1);
                        if (seq2[1] == '~') return KEY_F(6);
                    }
                    if (seq2[0] == '8') {
                        read(STDIN_FILENO, &seq2[1], 1);
                        if (seq2[1] == '~') return KEY_F(7);
                    }
                    if (seq2[0] == '9') {
                        read(STDIN_FILENO, &seq2[1], 1);
                        if (seq2[1] == '~') return KEY_F(8);
                    }
                    break;
                }
                case '2': {
                    read(STDIN_FILENO, &seq2[0], 1);
                    if (seq2[0] == '0') {
                        read(STDIN_FILENO, &seq2[1], 1);
                        if (seq2[1] == '~') return KEY_F(9);
                    }
                    if (seq2[0] == '1') {
                        read(STDIN_FILENO, &seq2[1], 1);
                        if (seq2[1] == '~') return KEY_F(10);
                    }
                    if (seq2[0] == '3') {
                        read(STDIN_FILENO, &seq2[1], 1);
                        if (seq2[1] == '~') return KEY_F(11);
                    }
                    if (seq2[0] == '4') {
                        read(STDIN_FILENO, &seq2[1], 1);
                        if (seq2[1] == '~') return KEY_F(12);
                    }
                    break;
                }
            }
        }
    }

    return '\x1b'; // если ничего не сработало — просто ESC
}

int Terminal::getWindowRows() {
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    return ws.ws_row;
}

int Terminal::getWindowCols() {
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    return ws.ws_col;
}

