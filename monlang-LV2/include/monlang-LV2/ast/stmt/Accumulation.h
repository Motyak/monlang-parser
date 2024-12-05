#ifndef AST_ACCUMULATION_H
#define AST_ACCUMULATION_H

#include <monlang-LV2/ast/expr/Lvalue.h>
#include <monlang-LV2/ast/Expression.h>

using identifier_t = std::string;

struct Accumulation {
    Lvalue variable;
    identifier_t operator_;
    Expression value;
};

#endif // AST_ACCUMULATION_H
