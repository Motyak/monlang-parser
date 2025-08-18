#ifndef AST_LIST_LITERAL_H
#define AST_LIST_LITERAL_H

#include <monlang-LV2/ast/Expression.h>

#include <vector>
#include <optional>
#include <any>

struct ListLiteral {
    std::vector<Expression> arguments;

    std::optional<std::any> _msbg = std::nullopt; // MultilineSquareBracketsGroup
    size_t _tokenLen = 0;
    size_t _tokenId = 123456789;
    ListLiteral() = default;
    ListLiteral(const std::vector<Expression>&);
};

#endif // AST_LIST_LITERAL_H
