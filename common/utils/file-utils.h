#ifndef FILE_UTILS_H
#define FILE_UTILS_H

inline std::string slurp_file(std::string filepath) {
    std::ifstream ifs(filepath);
    std::ostringstream oss;
    oss << ifs.rdbuf();
    return oss.str();
}

#endif // FILE_UTILS_H
