#ifndef AST_LIST_LITERAL_H
#define AST_LIST_LITERAL_H

#include <monlang-LV2/ast/Expression.h>

#include <vector>

struct ListLiteral {
    std::vector<Expression> arguments;

    size_t _tokenLen = 0;
    ListLiteral() = default;
    ListLiteral(const std::vector<Expression>&);
};

#endif // AST_LIST_LITERAL_H
