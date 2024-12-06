#ifndef FOREACH_STATEMENT_H
#define FOREACH_STATEMENT_H

#include <monlang-LV2/ast/stmt/ForeachStatement.h>

#include <monlang-LV2/common.h>
#include <monlang-LV2/Expression.h>
#include <monlang-LV2/expr/BlockExpression.h>

#include <monlang-LV1/ast/Program.h>

using ForeachBlock = BlockExpression;

template <>
struct MayFail_<ForeachStatement> {
    MayFail<Expression_> iterable;
    MayFail<MayFail_<ForeachBlock>> block;

    MayFail_() = default;
    explicit MayFail_(MayFail<Expression_>, MayFail<MayFail_<ForeachBlock>>);

    explicit MayFail_(ForeachStatement);
    explicit operator ForeachStatement() const;
};

bool peekForeachStatement(const ProgramSentence&);

MayFail<MayFail_<ForeachStatement>> consumeForeachStatement(LV1::Program&);

template <>
ForeachStatement unwrap(const MayFail_<ForeachStatement>&);

template <>
MayFail_<ForeachStatement> wrap(const ForeachStatement&);

#endif // FOREACH_STATEMENT_H
