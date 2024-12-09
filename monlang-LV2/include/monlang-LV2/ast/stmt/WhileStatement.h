#ifndef AST_WHILE_STATEMENT_H
#define AST_WHILE_STATEMENT_H

#include <monlang-LV2/ast/Expression.h>
#include <monlang-LV2/ast/expr/BlockExpression.h>

using WhileBlock = BlockExpression;

struct WhileStatement {
    Expression condition;
    WhileBlock block;
    bool until_loop = false;
};

struct DoWhileStatement {
    WhileBlock block;
    Expression condition;
    bool until_loop = false;
};

#endif // AST_WHILE_STATEMENT_H
