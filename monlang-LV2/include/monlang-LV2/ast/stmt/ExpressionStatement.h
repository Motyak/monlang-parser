#ifndef LV2_EXPRESSION_STATEMENT_H
#define LV2_EXPRESSION_STATEMENT_H

#include <monlang-LV2/ast/Expression.h>

#include <cstddef>
#include <optional>

struct ExpressionStatement {
    std::optional<Expression> expression;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    size_t _tokenId = 123456789;
    ExpressionStatement() = default;
    ExpressionStatement(const std::optional<Expression>&);
};

#endif // LV2_EXPRESSION_STATEMENT_H
