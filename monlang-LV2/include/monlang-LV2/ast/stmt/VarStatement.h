#ifndef AST_VAR_STATEMENT_H
#define AST_VAR_STATEMENT_H

#include <monlang-LV2/ast/Expression.h>

using identifier_t = std::string;

struct VarStatement {
    static const Atom KEYWORD;

    identifier_t identifier;
    Expression value;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    VarStatement() = default;
    VarStatement(const identifier_t&, const Expression&);
};

#endif // AST_VAR_STATEMENT_H
