#ifndef FOREACH_STATEMENT_H
#define FOREACH_STATEMENT_H

#include <monlang-LV2/Expression.h>
#include <monlang-LV2/expr/BlockExpression.h>
#include <monlang-LV2/context_init.h>

using ForeachBlock = BlockExpression;

struct ForeachStatement {
    Expression iterable;
    ForeachBlock block;
};

bool peekForeachStatement(const ProgramSentence&);

ForeachStatement buildForeachStatement(const ProgramSentence&, const context_t& = context_init_t{});

#endif // FOREACH_STATEMENT_H
