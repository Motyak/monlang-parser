#ifndef AST_MAP_LITERAL_H
#define AST_MAP_LITERAL_H

#include <monlang-LV2/ast/Expression.h>

#include <vector>
#include <optional>
#include <any>

struct MapLiteral {
    using Argument = std::pair<Expression, Expression>;
    std::vector<Argument> arguments;

    std::optional<std::any> _msbg = std::nullopt; // MultilineSquareBracketsGroup
    size_t _tokenLen = 0;
    size_t _tokenId = 123456789;
    MapLiteral() = default;
    MapLiteral(const std::vector<Argument>&);
};

#endif // AST_MAP_LITERAL_H
