#include "terminal.h"
#include "fs.h"
#include "panel.h"
#include <iostream>
int main() {
    Terminal term;

    int totalWidth = term.getWindowCols();
    int totalHeight = term.getWindowRows();

    int panelHeight = totalHeight - 2;
    int panelWidth  = (totalWidth) / 2;

    Panel left(0, 1, panelWidth-2, panelHeight);
    Panel right(panelWidth, 1, panelWidth, panelHeight);

    int focusedPanel = 0; // 0 = left, 1 = right

    while (true) {
        term.clearScreen();

        left.draw(term, focusedPanel == 0);
        right.draw(term, focusedPanel == 1);

        // Разделитель
        for (int i = 1; i <= panelHeight; i++) {
            term.moveCursor(i, panelWidth );
            std::cout << "";
        }

        // Нижняя строка
        term.moveCursor(totalHeight, 1);
        std::cout << "F5 Copy | Tab Switch | Enter Open | q Quit";

        term.flush();

        int key = term.readKey();

        Panel& active   = (focusedPanel == 0) ? left : right;
        Panel& inactive = (focusedPanel == 0) ? right : left;

        switch (key) {

            case 'q':
                return 0;

            case KEY_UP:
                active.moveSelection(-1);
                break;

            case KEY_DOWN:
                active.moveSelection(+1);
                break;

            case KEY_ENTER:
           // case '\n':
           // case '\r':
                active.enter();
                break;

            case '\t': // TAB
                focusedPanel = 1 - focusedPanel;
                break;

            case KEY_F(5): {
                const DirEntry& entry = active.getSelectedEntry();

                std::string src = active.getCurrentPath() + "/" + entry.name;
                std::string dst = inactive.getCurrentPath() + "/" + entry.name;

                if (!copyEntry(src, dst)) {
                    term.moveCursor(totalHeight, 1);
                    std::cout << "Copy failed!";
                } else {
                    inactive.refresh();
                }
                break;
            }

            default:
                break;
        }
    }

    return 0;
}
