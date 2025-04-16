#ifndef IOSTREAM_UTILS_H
#define IOSTREAM_UTILS_H

#include <iostream>

inline std::string slurp_stdin(bool repeatable=true) {
    auto res = std::string(
        std::istreambuf_iterator<char>(std::cin),
        std::istreambuf_iterator<char>()
    );

    if (repeatable) {
        std::cin.clear(); // reset EOF for std::istream
        std::clearerr(stdin); // reset EOF for std::FILE stream
    }

    return res;
}

#endif // IOSTREAM_UTILS_H
