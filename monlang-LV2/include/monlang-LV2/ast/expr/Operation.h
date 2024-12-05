#ifndef AST_OPERATION_H
#define AST_OPERATION_H

#include <monlang-LV2/ast/Expression.h>

using identifier_t = std::string;

struct Operation {
    Expression leftOperand;
    identifier_t operator_; // should we define a specific 'binary operator' type ?
    Expression rightOperand;
};

#endif // AST_OPERATION_H
