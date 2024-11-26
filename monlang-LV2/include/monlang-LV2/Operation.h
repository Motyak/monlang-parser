#ifndef OPERATION_H
#define OPERATION_H

#include <monlang-LV2/Expression.h>
#include <monlang-LV2/Lvalue.h>
#include <monlang-LV2/context.h>

using identifier_t = std::string;

struct Operation {
    Expression leftOperand;
    identifier_t operator_; // should we define a specific 'binary operator' type ?
    Expression rightOperand;
};

bool peekOperation(const Term&);

Operation buildOperation(const Term&, const context_t&);

#endif // OPERATION_H
