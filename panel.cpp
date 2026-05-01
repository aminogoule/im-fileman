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

void Panel::draw(Terminal& term, bool focused) const {
    int innerWidth = width_-2;
    std::string title = " " + path_ + " ";
    
    // Обрезаем заголовок, если он слишком длинный
    if ((int)title.size() > innerWidth)
        title = title.substr(0, innerWidth);

    int titleStart = (innerWidth - title.size()) / 2;

    // Верхняя рамка с заголовком
    term.moveCursor(y_, x_);
    std::cout << "╔";
    for (int i = 0; i < innerWidth;i++) {
        if (i == titleStart)
            std::cout << title;
        if (i < titleStart || i >= titleStart + (int)title.size())
            std::cout << "═";
    }
    std::cout << "╗";

    // Содержимое
    for (int i = 0; i < height_ - 2; ++i) {
        term.moveCursor(y_ + 1 + i, x_);
        std::cout << "║";

        std::string line(innerWidth, ' '); //bottom line fill

        if (i < (int)entries_.size()) {
            const auto& entry = entries_[i];
            std::ostringstream oss;

            if (i == selected_ && focused)
                oss << "\033[7m";  // обратное видео

            oss << " "; //pre line fill
            if (entry.isDir)
                oss << "[D]";
            else
                oss << "  ";
            oss << entry.name;

            if (i == selected_ && focused)
                oss << "\033[0m";

            line = oss.str().substr(0, innerWidth);
            //if ((int)line.size() < innerWidth)
            //    line += std::string(innerWidth - line.size(), ' ');
    	    std::cout<<line<<"║";
        }
        
        //std::cout << line;
        //std::cout << "║";
    }

    // Нижняя рамка
    term.moveCursor(y_ + height_ - 1, x_);
    std::cout << "╚" << std::string(innerWidth, '=') << "╝";
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

std::string Panel::getCurrentPath() const {
    return path_;
}
void Panel::refresh()
{
	load(currentPath_);
}

