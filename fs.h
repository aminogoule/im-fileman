#pragma once
#include <string>
#include <vector>

struct DirEntry {
    std::string name;
    bool isDir;
};

std::vector<DirEntry> listDirectory(const std::string& path);
bool changeDirectory(const std::string& path);
std::string getCurrentDirectory();
bool copyEntry(const std::string& src, const std::string& dst);
