#ifndef AST_VAR_STATEMENT_H
#define AST_VAR_STATEMENT_H

#include <monlang-LV2/ast/expr/Symbol.h>
#include <monlang-LV2/ast/Expression.h>

struct VarStatement {
    static const Atom KEYWORD;

    Symbol identifier;
    Expression value;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    VarStatement() = default;
    VarStatement(const Symbol&, const Expression&);
};

#endif // AST_VAR_STATEMENT_H
