#ifndef AST_LVALUE_H
#define AST_LVALUE_H

#include <string>

struct Lvalue {
    std::string identifier; // only identifier supported for now
};

#endif // AST_LVALUE_H
