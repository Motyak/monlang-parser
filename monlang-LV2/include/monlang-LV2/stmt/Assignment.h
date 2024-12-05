#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include <monlang-LV2/ast/stmt/Assignment.h>

#include <monlang-LV2/common.h>
#include <monlang-LV2/Expression.h>

#include <monlang-LV1/ast/Program.h>

template <>
struct MayFail_<Assignment> {
    MayFail<Lvalue> variable;
    MayFail<Expression_> value;
};

bool peekAssignment(const ProgramSentence&);

MayFail<MayFail_<Assignment>> consumeAssignment(LV1::Program&);

template <>
Assignment unwrap(const MayFail_<Assignment>&);

template <>
MayFail_<Assignment> wrap(const Assignment&);

#endif // ASSIGNMENT_H
