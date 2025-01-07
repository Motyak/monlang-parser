#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <fstream>

class CantOpenFileException : std::exception {
  public:
    std::string filename;
    CantOpenFileException(const std::string& filename) : std::exception(), filename(filename){}
    const char* what() const noexcept override {
        return ("Failed to open file `" + filename + "`").c_str();
    }
};

inline std::string slurp_file(std::string filename) {
    auto file = std::ifstream(filename);

    if (!file.is_open()) {
        throw CantOpenFileException(filename);
    }

    return std::string(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}

#endif // FILE_UTILS_H
