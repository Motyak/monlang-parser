#ifndef LAMBDA_H
#define LAMBDA_H

#include <monlang-LV2/ast/expr/Lambda.h>

#include <monlang-LV2/common.h>
#include <monlang-LV2/expr/BlockExpression.h> // MayFail_<LambdaBlock>

#include <monlang-LV1/ast/Word.h>

template <>
struct MayFail_<Lambda> {
    std::vector<identifier_t> parameters;
    MayFail_<LambdaBlock> body;

    MayFail_() = default;
    explicit MayFail_(std::vector<identifier_t>, MayFail_<LambdaBlock>);

    explicit MayFail_(Lambda);
    explicit operator Lambda() const;
};

bool peekLambda(const Word&);

MayFail<MayFail_<Lambda>> buildLambda(const Word&);

template <>
Lambda unwrap(const MayFail_<Lambda>&);

template <>
MayFail_<Lambda> wrap(const Lambda&);

#endif // LAMBDA_H
