#ifndef AST_NULL_STMT_H
#define AST_NULL_STMT_H

#include <monlang-LV2/ast/Lvalue.h>
#include <monlang-LV2/ast/Expression.h>

#include <string>
#include <cstddef>

struct NullStatement {
    static const std::string KEYWORD;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    size_t _tokenId = 123456789;
    NullStatement() = default;
};

#endif // AST_NULL_STMT_H
