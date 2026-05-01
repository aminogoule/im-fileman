#pragma once
#include "fs.h"
#include "terminal.h"
#include <string>
#include <vector>

class Panel {
public:
    Panel(int x, int y, int width, int height);
    //const std::string& getCurrentPath() const {
    //    return currentPath_;
    //}
    void load(const std::string& path);
    void refresh();
    void draw(Terminal& term, bool focused) const;
    void moveSelection(int delta);
    bool enter();
    std::string getCurrentPath() const;
    const DirEntry& getSelectedEntry() const {
        return entries_[selected_];
    }
private:
    std::string currentPath_;
    int x_, y_, width_, height_;
    std::string path_;
    std::vector<DirEntry> entries_;
    int selected_;
};

