#ifndef VAR_STATEMENT_H
#define VAR_STATEMENT_H

#include <monlang-LV2/ast/stmt/VarStatement.h>

#include <monlang-LV2/common.h>
#include <monlang-LV2/Expression.h>

#include <monlang-LV1/ast/Program.h>

template <>
struct MayFail_<VarStatement> {
    identifier_t identifier;
    MayFail<Expression_> value;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    MayFail_() = default;
    explicit MayFail_(identifier_t, MayFail<Expression_>);

    explicit MayFail_(const VarStatement&);
    explicit operator VarStatement() const;
};

bool peekVarStatement(const ProgramSentence&);

MayFail<MayFail_<VarStatement>> consumeVarStatement(LV1::Program&);

#endif // VAR_STATEMENT_H
