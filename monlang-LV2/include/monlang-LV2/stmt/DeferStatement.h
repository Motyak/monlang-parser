#ifndef DEFER_STATEMENT_H
#define DEFER_STATEMENT_H

#include <monlang-LV2/expr/BlockExpression.h>
#include <monlang-LV2/context.h>

using DeferBlock = BlockExpression;

struct DeferStatement {
    DeferBlock block;
};

bool peekDeferStatement(const ProgramSentence&);

DeferStatement buildDeferStatement(const ProgramSentence&, context_t* = new context_t{});

#endif // DEFER_STATEMENT_H
