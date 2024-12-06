#ifndef LET_STATEMENT_H
#define LET_STATEMENT_H

#include <monlang-LV2/ast/stmt/LetStatement.h>

#include <monlang-LV2/common.h>
#include <monlang-LV2/Expression.h>

#include <monlang-LV1/ast/Program.h>

template <>
struct MayFail_<LetStatement> {
    identifier_t identifier;
    MayFail<Expression_> value;

    MayFail_() = default;
    explicit MayFail_(identifier_t, MayFail<Expression_>);

    explicit MayFail_(LetStatement);
    explicit operator LetStatement() const;
};

bool peekLetStatement(const ProgramSentence&);

MayFail<MayFail_<LetStatement>> consumeLetStatement(LV1::Program&);

template <>
LetStatement unwrap(const MayFail_<LetStatement>&);

template <>
MayFail_<LetStatement> wrap(const LetStatement&);

#endif // LET_STATEMENT_H
