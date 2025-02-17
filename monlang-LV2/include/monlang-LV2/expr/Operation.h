#ifndef OPERATION_H
#define OPERATION_H

#include <monlang-LV2/ast/expr/Operation.h>

#include <monlang-LV2/Expression.h>

#include <monlang-LV1/ast/Term.h>

template <>
struct MayFail_<Operation> {
    MayFail<Expression_> leftOperand;
    Symbol operator_; // should we define a specific 'binary operator' type ?
                      //   -> maybe a _operator field set to true if found in precedence table ?
    MayFail<Expression_> rightOperand;

    size_t _tokenLen = 0;
    size_t _groupNesting = 0;
    MayFail_() = default;
    explicit MayFail_(const MayFail<Expression_>&, const Symbol&, const MayFail<Expression_>&);

    explicit MayFail_(const Operation&);
    explicit operator Operation() const;
};

bool peekOperation(const Term&);

MayFail<MayFail_<Operation>> buildOperation(const Term&);

#endif // OPERATION_H
