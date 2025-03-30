#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <monlang-LV2/ast/Expression.h>

#include <monlang-LV2/common.h>
#include <monlang-LV1/ast/Term.h>

struct _StubExpression_ {
    size_t _groupNesting = 0;
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
    MayFail_<MapLiteral>*,
    MayFail_<ListLiteral>*,
    StrLiteral*,
    Numeral*,
    SpecialSymbol*,
    Symbol*
>;

MayFail<Expression_> buildExpression(const Term&);

Expression_ StubExpression_();

Expression unwrap_expr(Expression_);
Expression_ wrap_expr(Expression);

bool is_lvalue(Expression_);

void delete_(Expression_);

#endif // EXPRESSION_H
