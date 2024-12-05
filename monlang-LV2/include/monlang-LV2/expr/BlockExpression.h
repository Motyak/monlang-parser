#ifndef BLOCK_EXPRESSION_H
#define BLOCK_EXPRESSION_H

#include <monlang-LV2/ast/expr/BlockExpression.h>

#include <monlang-LV2/common.h>
#include <monlang-LV2/Statement.h>

#include <monlang-LV1/ast/Word.h>

template <>
struct MayFail_<BlockExpression> {
    std::vector<MayFail<Statement_>> statements;
};

bool peekBlockExpression(const Word&);

MayFail<MayFail_<BlockExpression>> buildBlockExpression(const Word&);

template <>
BlockExpression unwrap(const MayFail_<BlockExpression>&);

template <>
MayFail_<BlockExpression> wrap(const BlockExpression&);

#endif // BLOCK_EXPRESSION_H
