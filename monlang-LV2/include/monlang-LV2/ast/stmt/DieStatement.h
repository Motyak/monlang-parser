#ifndef AST_DIE_STATEMENT_H
#define AST_DIE_STATEMENT_H

#include <monlang-LV1/ast/Atom.h>

struct DieStatement {
    static const Atom KEYWORD;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    DieStatement() = default;
};

#endif // AST_DIE_STATEMENT_H
