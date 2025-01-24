#ifndef OPERATION_H
#define OPERATION_H

#include <monlang-LV2/ast/expr/Operation.h>

#include <monlang-LV2/Expression.h>

#include <monlang-LV1/ast/Term.h>

using identifier_t = std::string;

template <>
struct MayFail_<Operation> {
    MayFail<Expression_> leftOperand;
    identifier_t operator_; // should we define a specific 'binary operator' type ?
    MayFail<Expression_> rightOperand;

    size_t _tokenLen = 0;
    size_t _groupNesting = 0;
    MayFail_() = default;
    explicit MayFail_(MayFail<Expression_>, identifier_t, MayFail<Expression_>);

    explicit MayFail_(Operation);
    explicit operator Operation() const;
};

bool peekOperation(const Term&);

MayFail<MayFail_<Operation>> buildOperation(const Term&);

#endif // OPERATION_H
