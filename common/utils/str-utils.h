#ifndef STR_UTILS_H
#define STR_UTILS_H

#include <string>
#include <vector>
#include <sstream>

inline std::string replace_all(const std::string& str, const std::string& from, const std::string& to)
{
    std::string _str = str; // working variable, needs to be mutable for ::replace() to work

    size_t start_pos = 0;
    while ((start_pos = _str.find(from, start_pos)) != std::string::npos)
    {
        _str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return _str;
}

#define escape_antislashes(str) \
    replace_all(str, std::string(1, '\\'), "\\\\")

#define escape_newlines(str) \
    replace_all(str, std::string(1, '\n'), "\\n")

#define escape_double_quotes(str) \
    replace_all(str, std::string(1, '"'), "\\\"")

inline std::vector<std::string> split(const std::string& str, const std::string& delim) {
    std::vector<std::string> res;

    size_t start_pos = 0;
    size_t end_pos = 0;
    while ((end_pos = str.find(delim, start_pos)) != std::string::npos)
    {
        res.push_back(str.substr(start_pos, end_pos - start_pos));

        // setting next start_pos as current end_pos past delim
        start_pos = end_pos + delim.size();
    }
    res.push_back(str.substr(start_pos));

    return res;
}

inline std::vector<std::string> split_in_two(const std::string& str, const std::string& delim) {
    std::vector<std::string> res;

    size_t start_pos = 0;
    size_t end_pos = 0;
    while ((end_pos = str.find(delim, start_pos)) != std::string::npos)
    {
        res.push_back(str.substr(start_pos, end_pos - start_pos));

        // setting next start_pos as current end_pos past delim
        start_pos = end_pos + delim.size();
        break; //
    }
    res.push_back(str.substr(start_pos));

    return res;
}

// to combine with result of split_in_two
template <typename T>
inline std::optional<T> second(const std::vector<T>& vec) {
    return vec.size() == 2? std::make_optional(vec.at(1)) : std::nullopt;
}

template <typename T>
inline std::string join(const std::vector<std::string>& vec, const T& delimiter) {
    std::stringstream ss;
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i != 0) {
            ss << delimiter;
        }
        ss << vec[i];
    }
    return ss.str();
}

#define INT2CSTR(num) (std::to_string(num).c_str())

inline std::string rjust(const std::string& str, size_t width, char fillchar=' ') {
    if (str.size() >= width) {
        return str;
    }
    return std::string(width - str.size(), fillchar) + str;
}

inline std::string rjust(size_t nb, size_t width, char fillchar=' ') {
    return rjust(std::to_string(nb), width, fillchar);
}

#endif // STR_UTILS_H
