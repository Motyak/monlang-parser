#ifndef AST_WHILE_STATEMENT_H
#define AST_WHILE_STATEMENT_H

#include <monlang-LV2/ast/Expression.h>
#include <monlang-LV2/ast/expr/BlockExpression.h>

using WhileBlock = BlockExpression;

struct WhileStatement {
    Expression condition;
    WhileBlock block;
    bool iterate_at_least_once = false;
};

#endif // AST_WHILE_STATEMENT_H
