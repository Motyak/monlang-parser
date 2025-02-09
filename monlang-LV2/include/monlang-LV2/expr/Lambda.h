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

    size_t _tokenLen = 0;
    size_t _groupNesting = 0;
    MayFail_() = default;
    explicit MayFail_(std::vector<identifier_t>, MayFail_<LambdaBlock>);

    explicit MayFail_(Lambda);
    explicit operator Lambda() const;
};

bool peekLambda(const Word&);

MayFail<MayFail_<Lambda>> buildLambda(const Word&);

#endif // LAMBDA_H
