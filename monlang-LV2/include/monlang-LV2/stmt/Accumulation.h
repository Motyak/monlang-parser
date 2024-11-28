#ifndef ACCUMULATION_H
#define ACCUMULATION_H

#include <monlang-LV2/expr/Lvalue.h>
#include <monlang-LV2/Expression.h>

using identifier_t = std::string;

struct Accumulation {
    Lvalue lhs;
    identifier_t operator_;
    Expression rhs;
};

#endif // ACCUMULATION_H
