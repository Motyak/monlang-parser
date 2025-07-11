#ifndef EXPRESSION_STATEMENT_H
#define EXPRESSION_STATEMENT_H

#include <monlang-LV2/ast/stmt/ExpressionStatement.h>

#include <monlang-LV2/common.h>
#include <monlang-LV2/Expression.h>

#include <monlang-LV1/ast/Program.h>

template <>
struct MayFail_<ExpressionStatement> {
    std::optional<MayFail<Expression_>> expression;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    size_t _tokenId = 123456789;
    MayFail_() = default;
    explicit MayFail_(const std::optional<MayFail<Expression_>>&);

    explicit MayFail_(ExpressionStatement);
    explicit operator ExpressionStatement() const;
};

MayFail<MayFail_<ExpressionStatement>> consumeExpressionStatement(LV1::Program&);

#endif // EXPRESSION_STATEMENT_H
