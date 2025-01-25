#ifndef AST_FOREACH_STATEMENT_H
#define AST_FOREACH_STATEMENT_H

#include <monlang-LV2/ast/Expression.h>
#include <monlang-LV2/ast/expr/BlockExpression.h>

using ForeachBlock = BlockExpression;

struct ForeachStatement {
    static const Atom KEYWORD;

    Expression iterable;
    ForeachBlock block;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    ForeachStatement() = default;
    ForeachStatement(const Expression&, const ForeachBlock&);
};

#endif // FOREACH_STATEMENT_H
