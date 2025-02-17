#ifndef ACCUMULATION_H
#define ACCUMULATION_H

#include <monlang-LV2/ast/stmt/Accumulation.h>

#include <monlang-LV2/common.h>
#include <monlang-LV2/Expression.h>

#include <monlang-LV1/ast/Program.h>

template <>
struct MayFail_<Accumulation> {
    Lvalue variable;
    Symbol operator_;
    MayFail<Expression_> value;

    Atom SEPARATOR(); // operator_ + SUFFIX

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;

    MayFail_() = default;
    explicit MayFail_(const Lvalue&, const Symbol&, const MayFail<Expression_>&);

    explicit MayFail_(const Accumulation&);
    explicit operator Accumulation() const;
};

bool peekAccumulation(const ProgramSentence&);

MayFail<MayFail_<Accumulation>> consumeAccumulation(LV1::Program&);

#endif // ACCUMULATION_H
