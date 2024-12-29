#ifndef AST_ASSIGNMENT_H
#define AST_ASSIGNMENT_H

#include <monlang-LV2/ast/expr/Lvalue.h>
#include <monlang-LV2/ast/Expression.h>

struct Assignment {
    Lvalue variable;
    Expression value;

    size_t _tokenLen = 0;
    Assignment() = default;
    Assignment(const Lvalue&, const Expression&);
};

#endif // AST_ASSIGNMENT_H
