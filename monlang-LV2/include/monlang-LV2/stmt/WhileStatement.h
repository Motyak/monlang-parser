#ifndef WHILE_STATEMENT_H
#define WHILE_STATEMENT_H

#include <monlang-LV2/ast/stmt/WhileStatement.h>

#include <monlang-LV2/Expression.h>
#include <monlang-LV2/expr/BlockExpression.h> // WhileBlock

#include <monlang-LV1/ast/Program.h>

template <>
struct MayFail_<WhileStatement> {
    MayFail<Expression_> condition;
    MayFail<MayFail_<WhileBlock>> block;
    bool until_loop = false;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    MayFail_() = default;
    explicit MayFail_(const MayFail<Expression_>&, const MayFail<MayFail_<WhileBlock>>&, bool);

    explicit MayFail_(const WhileStatement&);
    explicit operator WhileStatement() const;
};

template <>
struct MayFail_<DoWhileStatement> {
    MayFail<MayFail_<WhileBlock>> block;
    MayFail<Expression_> condition;
    bool until_loop = false;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    MayFail_() = default;
    explicit MayFail_(const MayFail<MayFail_<WhileBlock>>&, const MayFail<Expression_>&, bool);

    explicit MayFail_(const DoWhileStatement&);
    explicit operator DoWhileStatement() const;
};

bool peekWhileStatement(const ProgramSentence&);
bool peekDoWhileStatement(const ProgramSentence&);

MayFail<MayFail_<WhileStatement>> consumeWhileStatement(LV1::Program&);
MayFail<MayFail_<DoWhileStatement>> consumeDoWhileStatement(LV1::Program&);

#endif // WHILE_STATEMENT_H
