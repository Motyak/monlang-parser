#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <fstream>

inline std::string slurp_file(std::string filename) {
    auto file = std::ifstream(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file `" + filename + "`");
    }

    return std::string(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}

#endif // FILE_UTILS_H
