#ifndef GUARD_H
#define GUARD_H

#include <monlang-LV2/ast/stmt/Guard.h>

#include <monlang-LV2/common.h>
#include <monlang-LV2/Expression.h>

#include <monlang-LV1/ast/Program.h>

#define JUMP_STATEMENTS_ MayFail<MayFail_<ReturnStatement>>*, MayFail<BreakStatement>*, \
                         MayFail<ContinueStatement>*, MayFail<DieStatement>*

template <>
struct MayFail_<Guard> {
    MayFail<Expression_> condition;
    std::variant<JUMP_STATEMENTS_> jump;

    MayFail_() = default;
    explicit MayFail_(MayFail<Expression_>, std::variant<JUMP_STATEMENTS_>);

    explicit MayFail_(Guard);
    explicit operator Guard() const;
};

bool peekGuard(const ProgramSentence&);

MayFail<MayFail_<Guard>> consumeGuard(LV1::Program&);

#endif // GUARD_H
