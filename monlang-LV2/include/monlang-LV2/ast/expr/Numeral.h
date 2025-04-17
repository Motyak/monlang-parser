#ifndef AST_NUMERAL_H
#define AST_NUMERAL_H

#include <string>

struct Numeral {
    std::string fmt;

    size_t _tokenLen = 0;
    size_t _groupNesting = 0; // special case, because can't be malformed
    Numeral() = default;
    Numeral(const std::string&);
};

#endif // AST_NUMERAL_H
