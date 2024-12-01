#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include <monlang-LV2/expr/Lvalue.h>
#include <monlang-LV2/Expression.h>
#include <monlang-LV2/context_init.h>

struct Assignment {
    Lvalue lhs;
    Expression rhs;
};

bool peekAssignment(const ProgramSentence&);

Assignment buildAssignment(const ProgramSentence&, const context_t& = context_init_t{});

#endif // ASSIGNMENT_H
