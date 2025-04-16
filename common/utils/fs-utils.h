#ifndef FS_UTILS_H
#define FS_UTILS_H

#include <filesystem>

inline bool dir_exists(const std::string& path) {
    return std::filesystem::is_directory(path);
}

#ifdef FS_UTILS_MAIN
#include <iostream>
// g++ -x c++ -D FS_UTILS_MAIN include/utils/fs-utils.h
int main() {
    if (dir_exists("out")) {
        std::cout << "yes" << std::endl;
    }
}
#endif

#endif // FS_UTILS_H
