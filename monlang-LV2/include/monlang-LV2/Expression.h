#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <monlang-LV2/ast/Expression.h>

#include <monlang-LV2/common.h>
#include <monlang-LV1/ast/Term.h>

using Expression_ = std::variant<
    MayFail_<Operation>*,

    /// LANGUAGE DEFINED EXPRESSIONS ///

    MayFail_<FunctionCall>*,
    MayFail_<Lambda>*,
    MayFail_<BlockExpression>*,
    Literal*,
    SpecialSymbol*,

    ////////////////////////////////////

    // fall-through expression
    Lvalue*
>;

MayFail<Expression_> buildExpression(const Term&);

Expression unwrap_expr(Expression_);
Expression_ wrap_expr(Expression);

#endif // EXPRESSION_H
