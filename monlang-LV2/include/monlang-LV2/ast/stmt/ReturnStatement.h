#ifndef AST_RETURN_STATEMENT_H
#define AST_RETURN_STATEMENT_H

#include <monlang-LV2/ast/Expression.h>

#include <optional>
#include <string>
#include <cstddef>

struct ReturnStatement {
    static const std::string KEYWORD;

    std::optional<Expression> value;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    size_t _tokenId = 123456789;
    ReturnStatement() = default;
    ReturnStatement(const std::optional<Expression>&);
};

#endif // AST_RETURN_STATEMENT_H
