#ifndef STR_UTILS_H
#define STR_UTILS_H

#include <string>
#include <vector>

inline std::string replace_all(const std::string& str, const std::string& from, const std::string& to)
{
    std::string _str = str; // working variable, needs to be mutable for ::replace() to work

    size_t start_pos = 0;
    while ((start_pos = _str.find(from, start_pos)) != std::string::npos)
    {
        _str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

#define escape_newlines(str) (replace_all(str, std::string(1, /*LF*/char(10)), "\\n"))

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

#define itoa(num) (std::to_string(num).c_str())

#endif // STR_UTILS_H
