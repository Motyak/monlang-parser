#ifndef AST_FOREACH_STATEMENT_H
#define AST_FOREACH_STATEMENT_H

#include <monlang-LV2/ast/Expression.h>
#include <monlang-LV2/ast/expr/BlockExpression.h>

using ForeachBlock = BlockExpression;

struct ForeachStatement {
    Expression iterable;
    ForeachBlock block;
};

#endif // FOREACH_STATEMENT_H
