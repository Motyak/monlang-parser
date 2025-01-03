#include <iostream>

inline std::string slurp_stdin() {
    auto res = std::string(
        std::istreambuf_iterator<char>(std::cin),
        std::istreambuf_iterator<char>()
    );
    std::cin.clear(); // reset EOF for std::istream
    std::clearerr(stdin); // reset EOF for std::FILE stream
    return res;
}
