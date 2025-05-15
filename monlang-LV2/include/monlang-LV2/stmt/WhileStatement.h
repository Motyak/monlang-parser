#ifndef WHILE_STATEMENT_H
#define WHILE_STATEMENT_H

#include <monlang-LV2/ast/stmt/WhileStatement.h>

#include <monlang-LV2/Expression.h>
#include <monlang-LV2/expr/BlockExpression.h> // WhileBlock

#include <monlang-LV1/ast/Program.h>

#include <utils/stub-ctor.h>

template <>
struct MayFail_<WhileStatement> {
    MayFail<Expression_> condition;
    MayFail<MayFail_<WhileBlock>> block;
    bool until_loop = false;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    size_t _tokenId = 123456789;
    MayFail_() = default;
    explicit MayFail_(const MayFail<Expression_>&, const MayFail<MayFail_<WhileBlock>>&, bool);

    explicit MayFail_(const WhileStatement&);
    explicit operator WhileStatement() const;
};

/* compound statements, for further DoWhileStatement */
template <> struct MayFail_<C_DoStatement>;
template <> struct MayFail_<C_WhileStatement>;

template <>
struct MayFail_<C_DoStatement> {
    MayFail<MayFail_<WhileBlock>> block;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    size_t _tokenId = 123456789;
    MayFail_() = default;
    explicit MayFail_(const MayFail<MayFail_<WhileBlock>>&);

    explicit MayFail_(const C_DoStatement&);
    explicit operator C_DoStatement() const;
};

template <>
struct MayFail_<C_WhileStatement> {
    MayFail<Expression_> condition;
    bool until_loop = false;

    bool _stub = false;
    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    size_t _tokenId = 123456789;
    MayFail_() = default;
    explicit MayFail_(_dummy_stub); // sets _stub to true
    explicit MayFail_(const MayFail<Expression_>&, bool);

    explicit MayFail_(const C_WhileStatement&);
    explicit operator C_WhileStatement() const;
};

template <>
struct MayFail_<DoWhileStatement> {
    MayFail<MayFail_<C_DoStatement>> doStmt;
    MayFail<MayFail_<C_WhileStatement>> whileStmt;

    MayFail_() = default;
    explicit MayFail_(const MayFail<MayFail_<C_DoStatement>>&, const MayFail<MayFail_<C_WhileStatement>>&);

    explicit MayFail_(const DoWhileStatement&);
    explicit operator DoWhileStatement() const;
};

bool peekWhileStatement(const ProgramSentence&);
bool peekDoWhileStatement(const ProgramSentence&);

MayFail<MayFail_<WhileStatement>> consumeWhileStatement(LV1::Program&);
MayFail<MayFail_<DoWhileStatement>> consumeDoWhileStatement(LV1::Program&);

#endif // WHILE_STATEMENT_H
