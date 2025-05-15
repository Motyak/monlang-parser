#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include <monlang-LV2/ast/stmt/Assignment.h>
#include <monlang-LV2/Lvalue.h>

#include <monlang-LV2/common.h>
#include <monlang-LV2/Expression.h>

#include <monlang-LV1/ast/Program.h>

template <>
struct MayFail_<Assignment> {
    MayFail<Lvalue_> variable;
    MayFail<Expression_> value;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    size_t _tokenId = 123456789;
    MayFail_() = default;
    explicit MayFail_(const MayFail<Lvalue_>&, const MayFail<Expression_>&);

    explicit MayFail_(const Assignment&);
    explicit operator Assignment() const;
};

bool peekAssignment(const ProgramSentence&);

MayFail<MayFail_<Assignment>> consumeAssignment(LV1::Program&);

#endif // ASSIGNMENT_H
