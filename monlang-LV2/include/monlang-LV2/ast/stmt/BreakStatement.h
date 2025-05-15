#ifndef AST_BREAK_STATEMENT_H
#define AST_BREAK_STATEMENT_H

#include <string>
#include <cstddef>

struct BreakStatement {
    static const std::string KEYWORD;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    size_t _tokenId = 123456789;
    BreakStatement() = default;
};

#endif // AST_BREAK_STATEMENT_H
