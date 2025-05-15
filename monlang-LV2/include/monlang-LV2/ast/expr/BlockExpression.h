#ifndef AST_BLOCK_EXPRESSION_H
#define AST_BLOCK_EXPRESSION_H

#include <monlang-LV2/ast/Statement.h>

#include <vector>

struct BlockExpression {
    std::vector<Statement> statements;

    size_t _tokenLen = 0;
    size_t _tokenId = 123456789;
    bool _dollars = false;
    BlockExpression() = default;
    BlockExpression(const std::vector<Statement>&);
};

#endif // AST_BLOCK_EXPRESSION_H
