#ifndef AST_CONTINUE_STATEMENT_H
#define AST_CONTINUE_STATEMENT_H

#include <monlang-LV1/ast/Atom.h>

struct ContinueStatement {
    static const Atom KEYWORD;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    ContinueStatement() = default;
};

#endif // AST_CONTINUE_STATEMENT_H
