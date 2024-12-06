#ifndef ACCUMULATION_H
#define ACCUMULATION_H

#include <monlang-LV2/ast/stmt/Accumulation.h>

#include <monlang-LV2/common.h>
#include <monlang-LV2/Expression.h>

#include <monlang-LV1/ast/Program.h>

template <>
struct MayFail_<Accumulation> {
    Lvalue variable;
    identifier_t operator_;
    MayFail<Expression_> value;

    MayFail_() = default;
    explicit MayFail_(Lvalue, identifier_t, MayFail<Expression_>);

    explicit MayFail_(Accumulation);
    explicit operator Accumulation() const;
};

bool peekAccumulation(const ProgramSentence&);

MayFail<MayFail_<Accumulation>> consumeAccumulation(LV1::Program&);

template <>
Accumulation unwrap(const MayFail_<Accumulation>&);

template <>
MayFail_<Accumulation> wrap(const Accumulation&);

#endif // ACCUMULATION_H
