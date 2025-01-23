#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <monlang-LV2/ast/Expression.h>

#include <monlang-LV2/common.h>
#include <monlang-LV1/ast/Term.h>

struct _StubExpression_ {
    ;
};

using Expression_ = std::variant<
    _StubExpression_*, // returned by StubExpression_()
                       // .., to be used when the expression itself is malformed
                       // ..(cannot be associated with any type of expression)

    /////////////////////////////

    MayFail_<Operation>*,
    MayFail_<FunctionCall>*,
    MayFail_<Lambda>*,
    MayFail_<BlockExpression>*,
    Literal*,
    SpecialSymbol*,
    Lvalue*
>;

MayFail<Expression_> buildExpression(const Term&);

Expression_ StubExpression_();

Expression unwrap_expr(Expression_);
Expression_ wrap_expr(Expression);

#endif // EXPRESSION_H
