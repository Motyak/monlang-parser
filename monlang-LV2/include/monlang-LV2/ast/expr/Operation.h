#ifndef AST_OPERATION_H
#define AST_OPERATION_H

#include <monlang-LV2/ast/Expression.h>
#include <monlang-LV2/ast/expr/Symbol.h>

struct Operation {
    Expression leftOperand;
    Symbol operator_;
    Expression rightOperand;

    size_t _tokenLen = 0;
    Operation() = default;
    Operation(const Expression&, const Symbol&, const Expression&);
};

#endif // AST_OPERATION_H
