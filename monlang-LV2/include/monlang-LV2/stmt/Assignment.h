#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include <monlang-LV2/expr/Lvalue.h>
#include <monlang-LV2/Expression.h>
#include <monlang-LV2/context.h>

struct Assignment {
    Lvalue variable;
    Expression value;
};

bool peekAssignment(const ProgramSentence&);

Assignment buildAssignment(const ProgramSentence&, context_t* = new context_t{});

#endif // ASSIGNMENT_H
