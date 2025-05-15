#ifndef LV2_PROGRAM_H
#define LV2_PROGRAM_H

#include <monlang-LV2/ast/Program.h>

#include <monlang-LV2/Statement.h>

template <>
struct MayFail_<LV2::Program> {
    std::vector<MayFail<Statement_>> statements;

    size_t _tokenId = 123456789;
    MayFail_() = default;
    explicit MayFail_(const std::vector<MayFail<Statement_>>&);

    explicit MayFail_(LV2::Program);
    explicit operator LV2::Program() const;
};

MayFail<MayFail_<LV2::Program>> consumeProgram(LV1::Program&);

#endif // LV2_PROGRAM_H
