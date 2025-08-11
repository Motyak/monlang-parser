#ifndef LET_STATEMENT_H
#define LET_STATEMENT_H

#include <monlang-LV2/ast/stmt/LetStatement.h>

#include <monlang-LV2/common.h>
#include <monlang-LV2/Lvalue.h>

#include <monlang-LV1/ast/Program.h>

template <>
struct MayFail_<LetStatement> {
    Symbol alias;
    MayFail<Lvalue_> variable;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    size_t _tokenId = 123456789;
    MayFail_() = default;
    explicit MayFail_(const Symbol&, const MayFail<Lvalue_>&);

    explicit MayFail_(const LetStatement&);
    explicit operator LetStatement() const;
};

bool peekLetStatement(const ProgramSentence&);

MayFail<MayFail_<LetStatement>> consumeLetStatement(LV1::Program&);

#endif // LET_STATEMENT_H
