#include "panel.h"
#include <iostream>
#include <sstream>
#include <sys/param.h>

#if __FreeBSD_version>=1300000
	#include "unistd.h"
#endif

Panel::Panel(int x, int y, int width, int height)
    : x_(x), y_(y), width_(width), height_(height),
      path_(getCurrentDirectory()), selected_(0) {
    load(path_);
}

void Panel::load(const std::string& path) {
    path_ = path;
    entries_ = listDirectory(path_);
    selected_ = 0;
}

void Panel::moveSelection(int delta) {
    int newSel = selected_ + delta;
    if (newSel >= 0 && newSel < (int)entries_.size()) {
        selected_ = newSel;
    }
}

bool Panel::enter() {
    if (entries_.empty()) return false;
    const auto& entry = entries_[selected_];
    if (entry.isDir) {
        std::string full = path_ + "/" + entry.name;
        if (access(full.c_str(), X_OK) == 0 && changeDirectory(full)) {
            load(getCurrentDirectory());
            return true;
        }
    }
    return false;
}

// render main panels
void Panel::draw(Terminal& term, bool focused) const {
    const int innerWidth = width_ - 2;

    // ---- Header ---
    std::string title = " " + path_ + " ";
    if ((int)title.size() > innerWidth)
        title.resize(innerWidth);

    const int titleStart = (innerWidth - title.size()) / 2;

    term.moveCursor(y_, x_);
    std::cout << "╔";

    for (int i = 0; i < innerWidth;) {
        if (i == titleStart) {
            std::cout << title;
            i += title.size();
        } else {
            std::cout << "═";
            ++i;
        }
    }

    std::cout << "╗";

    // ---- Main Content ---
    for (int i = 0; i < height_ - 2; ++i) {
        term.moveCursor(y_ + 1 + i, x_);
        std::cout << "║";

        std::string line;
        line.reserve(innerWidth);

        if (i < (int)entries_.size()) {
            const auto& entry = entries_[i];

            line += ' ';
            line += (entry.isDir ? "[D]" : "  ");
            line += entry.name;
        }

        // Обрезка / выравнивание (ТОЛЬКО по видимым символам)
        if ((int)line.size() > innerWidth) {
            line.resize(innerWidth);
        } else {
            line.append(innerWidth - line.size(), ' ');
        }

        const bool isSelected = (i == selected_ && focused);

        // ANSI применяется ТОЛЬКО при выводе
        if (isSelected)
            std::cout << "\033[7m";

        std::cout << line;

        if (isSelected)
            std::cout << "\033[0m";

        std::cout << "║";
    }

    // ---- Нижняя рамка ----
    term.moveCursor(y_ + height_ - 1, x_);
    std::cout << "╚";

    for (int i = 0; i < innerWidth; ++i)
        std::cout << "═";

    std::cout << "╝";
}

std::string Panel::getCurrentPath() const {
    return path_;
}
void Panel::refresh()
{
	load(currentPath_);
}

