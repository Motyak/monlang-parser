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
    bool iterate_at_least_once = false;

    MayFail_() = default;
    explicit MayFail_(MayFail<Expression_>, MayFail<MayFail_<WhileBlock>>, bool, bool);

    explicit MayFail_(WhileStatement);
    explicit operator WhileStatement() const;
};

bool peekWhileStatement(const ProgramSentence&);
bool peekDoWhileStatement(const ProgramSentence&);

MayFail<MayFail_<WhileStatement>> consumeWhileStatement(LV1::Program&);
MayFail<MayFail_<WhileStatement>> consumeDoWhileStatement(LV1::Program&);

#endif // WHILE_STATEMENT_H
