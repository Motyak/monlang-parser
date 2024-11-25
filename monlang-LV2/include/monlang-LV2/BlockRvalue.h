#ifndef BLOCK_RVALUE_H
#define BLOCK_RVALUE_H

#include <monlang-LV2/Statement.h>

struct BlockRvalue {
    std::vector<Statement> statements;
};

bool peekBlockRvalue(const Word&);

BlockRvalue buildBlockRvalue(const Word&, const context_t& = context_t{});

#endif // BLOCK_RVALUE_H
