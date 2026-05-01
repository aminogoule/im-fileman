#include "fs.h"
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <fstream>

std::vector<DirEntry> listDirectory(const std::string& path) {
    std::vector<DirEntry> entries;
    DIR* dir = opendir(path.c_str());
    if (!dir) return entries;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") == 0) continue;
        DirEntry e;
        e.name = entry->d_name;

        struct stat st;
        if (stat((path + "/" + e.name).c_str(), &st) == 0) {
            e.isDir = S_ISDIR(st.st_mode);
        } else {
            e.isDir = false;
        }

        entries.push_back(e);
    }

    closedir(dir);
    return entries;
}

bool changeDirectory(const std::string& path) {
    return chdir(path.c_str()) == 0;
}

std::string getCurrentDirectory() {
    char buf[PATH_MAX];
    getcwd(buf, sizeof(buf));
    return std::string(buf);
}
static bool copyFile(const std::string& src, const std::string& dst) {
    std::ifstream in(src, std::ios::binary);
    std::ofstream out(dst, std::ios::binary);
    if (!in || !out) return false;

    out << in.rdbuf();
    return true;
}

static bool copyDir(const std::string& src, const std::string& dst) {
    mkdir(dst.c_str(), 0755);

    DIR* dir = opendir(src.c_str());
    if (!dir) return false;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") continue;

        std::string srcPath = src + "/" + name;
        std::string dstPath = dst + "/" + name;

        struct stat st;
        if (stat(srcPath.c_str(), &st) != 0) continue;

        if (S_ISDIR(st.st_mode)) {
            copyDir(srcPath, dstPath);
        } else if (S_ISREG(st.st_mode)) {
            copyFile(srcPath, dstPath);
        }
    }

    closedir(dir);
    return true;
}

bool copyEntry(const std::string& src, const std::string& dst) {
    struct stat st;
    if (stat(src.c_str(), &st) != 0) return false;

    if (S_ISDIR(st.st_mode))
        return copyDir(src, dst);
    else if (S_ISREG(st.st_mode))
        return copyFile(src, dst);
    else
        return false;
}
