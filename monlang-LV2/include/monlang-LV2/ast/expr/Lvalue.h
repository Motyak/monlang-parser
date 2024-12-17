#ifndef AST_LVALUE_H
#define AST_LVALUE_H

#include <string>

struct Lvalue {
    std::string identifier; // only identifier supported for now

    size_t _tokenLen = 0;
    Lvalue() = default;
    Lvalue(const std::string&);
};

#endif // AST_LVALUE_H
