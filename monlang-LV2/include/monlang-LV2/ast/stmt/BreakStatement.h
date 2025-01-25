#ifndef AST_BREAK_STATEMENT_H
#define AST_BREAK_STATEMENT_H

#include <monlang-LV1/ast/Atom.h>

struct BreakStatement {
    static const Atom KEYWORD;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    BreakStatement() = default;
};

#endif // AST_BREAK_STATEMENT_H
