#ifndef OPERATION_H
#define OPERATION_H

#include <monlang-LV2/ast/expr/Operation.h>

#include <monlang-LV2/common.h>
#include <monlang-LV2/Expression.h>

#include <monlang-LV1/ast/Term.h>

using identifier_t = std::string;

template <>
struct MayFail_<Operation> {
    MayFail<Expression_> leftOperand;
    identifier_t operator_; // should we define a specific 'binary operator' type ?
    MayFail<Expression_> rightOperand;
};

bool peekOperation(const Term&);

MayFail<MayFail_<Operation>> buildOperation(const Term&);

template <>
Operation unwrap(const MayFail_<Operation>&);

template <>
MayFail_<Operation> wrap(const Operation&);

#endif // OPERATION_H
