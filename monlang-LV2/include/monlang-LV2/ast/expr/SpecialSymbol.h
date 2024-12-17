#ifndef AST_SPECIAL_SYMBOL_H
#define AST_SPECIAL_SYMBOL_H

#include <string>

struct SpecialSymbol {
    std::string str;

    size_t _tokenLen = 0;
    SpecialSymbol() = default;
    SpecialSymbol(const std::string&);
};

#endif // AST_SPECIAL_SYMBOL_H
