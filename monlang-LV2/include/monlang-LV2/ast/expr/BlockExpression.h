#ifndef AST_BLOCK_EXPRESSION_H
#define AST_BLOCK_EXPRESSION_H

#include <monlang-LV2/ast/Statement.h>

#include <vector>

struct BlockExpression {
    std::vector<Statement> statements;
};

#endif // AST_BLOCK_EXPRESSION_H
