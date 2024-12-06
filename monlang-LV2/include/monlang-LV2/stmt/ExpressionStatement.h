#ifndef EXPRESSION_STATEMENT_H
#define EXPRESSION_STATEMENT_H

#include <monlang-LV2/ast/stmt/ExpressionStatement.h>

#include <monlang-LV2/common.h>
#include <monlang-LV2/Expression.h>

#include <monlang-LV1/ast/Program.h>

template <>
struct MayFail_<ExpressionStatement> {
    MayFail<Expression_> expression;

    MayFail_() = default;
    explicit MayFail_(MayFail<Expression_>);

    explicit MayFail_(ExpressionStatement);
    explicit operator ExpressionStatement() const;
};

MayFail<MayFail_<ExpressionStatement>> consumeExpressionStatement(LV1::Program&);

template <>
ExpressionStatement unwrap(const MayFail_<ExpressionStatement>&);

template <>
MayFail_<ExpressionStatement> wrap(const ExpressionStatement&);

#endif // EXPRESSION_STATEMENT_H
