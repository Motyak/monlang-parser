#ifndef DEFER_STATEMENT_H
#define DEFER_STATEMENT_H

#include <monlang-LV2/ast/stmt/DeferStatement.h>

#include <monlang-LV2/common.h>
#include <monlang-LV2/expr/BlockExpression.h>

template <>
struct MayFail_<DeferStatement> {
    MayFail_<DeferBlock> block;
};

bool peekDeferStatement(const ProgramSentence&);

MayFail<MayFail_<DeferStatement>> consumeDeferStatement(LV1::Program&);

template <>
DeferStatement unwrap(const MayFail_<DeferStatement>&);

template <>
MayFail_<DeferStatement> wrap(const DeferStatement&);

#endif // DEFER_STATEMENT_H
