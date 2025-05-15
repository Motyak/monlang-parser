#ifndef RETURN_STATEMENT_H
#define RETURN_STATEMENT_H

#include <monlang-LV2/ast/stmt/ReturnStatement.h>

#include <monlang-LV2/common.h>
#include <monlang-LV2/Expression.h>

#include <monlang-LV1/ast/Program.h>

template <>
struct MayFail_<ReturnStatement> {
    std::optional<MayFail<Expression_>> value;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    size_t _tokenId = 123456789;
    MayFail_() = default;
    explicit MayFail_(const std::optional<MayFail<Expression_>>&);

    explicit MayFail_(const ReturnStatement&);
    explicit operator ReturnStatement() const;
};

bool peekReturnStatement(const ProgramSentence&);

MayFail<MayFail_<ReturnStatement>> consumeReturnStatement(LV1::Program&);

#endif // RETURN_STATEMENT_H
