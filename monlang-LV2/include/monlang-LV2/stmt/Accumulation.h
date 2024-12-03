#ifndef ACCUMULATION_H
#define ACCUMULATION_H

#include <monlang-LV2/expr/Lvalue.h>
#include <monlang-LV2/Expression.h>
#include <monlang-LV2/context.h>

using identifier_t = std::string;

struct Accumulation {
    Lvalue lhs;
    identifier_t operator_;
    Expression rhs;
};

bool peekAccumulation(const ProgramSentence&);

Accumulation buildAccumulation(const ProgramSentence&, const context_t* = disposable_cx());

#endif // ACCUMULATION_H
