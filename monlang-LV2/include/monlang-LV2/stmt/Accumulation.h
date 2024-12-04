#ifndef ACCUMULATION_H
#define ACCUMULATION_H

#include <monlang-LV2/expr/Lvalue.h>
#include <monlang-LV2/Expression.h>
#include <monlang-LV2/context.h>

using identifier_t = std::string;

struct Accumulation {
    Lvalue variable;
    identifier_t operator_;
    Expression value;
};

bool peekAccumulation(const ProgramSentence&);

Accumulation buildAccumulation(const ProgramSentence&, context_t* = new context_t{});

#endif // ACCUMULATION_H
