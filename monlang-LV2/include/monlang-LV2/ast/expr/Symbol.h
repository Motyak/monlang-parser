#ifndef AST_SYMBOL_H
#define AST_SYMBOL_H

#include <string>

// empty str value serves to indicate a stub Symbol
struct Symbol {
    std::string name;

    size_t _tokenLen = 0;
    size_t _tokenId = 123456789;
    size_t _groupNesting = 0; // special case, because can't be malformed
    bool _lvalue = false;
    Symbol() = default;
    Symbol(const std::string&);
};

#endif // AST_SYMBOL_H
