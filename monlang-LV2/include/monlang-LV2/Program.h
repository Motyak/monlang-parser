#ifndef LV2_PROGRAM_H
#define LV2_PROGRAM_H

#include <monlang-LV2/ast/Program.h>

#include <monlang-LV2/Statement.h>

template <>
struct MayFail_<LV2::Program> {
    std::vector<MayFail<Statement_>> statements;

    MayFail_() = default;
    explicit MayFail_(std::vector<MayFail<Statement_>>);

    explicit MayFail_(LV2::Program);
    explicit operator LV2::Program() const;
};

MayFail<MayFail_<LV2::Program>> consumeProgram(LV1::Program&);

template <>
LV2::Program unwrap(const MayFail_<LV2::Program>&);

template <>
MayFail_<LV2::Program> wrap(const LV2::Program&);

#endif // LV2_PROGRAM_H
