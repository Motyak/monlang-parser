#ifndef BLOCK_EXPRESSION_H
#define BLOCK_EXPRESSION_H

#include <monlang-LV2/Statement.h>

struct BlockExpression {
    std::vector<Statement> statements;
};

bool peekBlockExpression(const Word&);

BlockExpression buildBlockExpression(const Word&, const context_t& = context_t{});

#endif // BLOCK_EXPRESSION_H
