#ifndef ARGS_UTILS_H
#define ARGS_UTILS_H

#include <utility>
#include <vector>
#include <string>

inline std::pair<
    /* from start to sep (excluded) */
    std::vector<std::string>,
    /* from sep+1 to end (included) */
    std::vector<std::string>
>
split_args_in_two(int argc, char* argv[], const std::string& sep) {
    std::vector<std::string> left;
    std::vector<std::string> right;

    int i = 1;
    for (; i < argc; ++i) {
        if (argv[i] == sep) {
            break;
        }
        left.push_back(argv[i]);
    }

    for (; i < argc; ++i) {
        right.push_back(argv[i]);
    }

    return std::make_pair(left, right);
}

#endif // ARGS_UTILS_H
