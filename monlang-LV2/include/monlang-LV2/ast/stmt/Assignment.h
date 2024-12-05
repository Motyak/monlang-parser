#ifndef AST_ASSIGNMENT_H
#define AST_ASSIGNMENT_H

#include <monlang-LV2/ast/expr/Lvalue.h>
#include <monlang-LV2/ast/Expression.h>

struct Assignment {
    Lvalue variable;
    Expression value;
};

#endif // AST_ASSIGNMENT_H
