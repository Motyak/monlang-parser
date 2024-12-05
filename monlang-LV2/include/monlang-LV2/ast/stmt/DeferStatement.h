#ifndef AST_DEFER_STATEMENT_H
#define AST_DEFER_STATEMENT_H

#include <monlang-LV2/ast/expr/BlockExpression.h>

using DeferBlock = BlockExpression;

struct DeferStatement {
    DeferBlock block;
};

#endif // AST_DEFER_STATEMENT_H
