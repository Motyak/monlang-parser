#ifndef AST_LITERAL_H
#define AST_LITERAL_H

#include <string>

struct Literal {
    std::string str;

    size_t _tokenLen = 0;
    Literal() = default;
    Literal(const std::string&);
};

#endif // AST_LITERAL_H
