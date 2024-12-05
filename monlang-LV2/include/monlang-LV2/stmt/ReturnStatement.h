#ifndef RETURN_STATEMENT_H
#define RETURN_STATEMENT_H

#include <monlang-LV2/ast/stmt/ReturnStatement.h>

#include <monlang-LV2/common.h>
#include <monlang-LV2/Expression.h>

#include <monlang-LV1/ast/Program.h>

template <>
struct MayFail_<ReturnStatement> {
    std::optional<MayFail<Expression_>> value;
};

bool peekReturnStatement(const ProgramSentence&);

MayFail<MayFail_<ReturnStatement>> consumeReturnStatement(LV1::Program&);

template <>
ReturnStatement unwrap(const MayFail_<ReturnStatement>&);

template <>
MayFail_<ReturnStatement> wrap(const ReturnStatement&);

#endif // RETURN_STATEMENT_H
