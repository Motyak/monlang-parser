#ifndef LAMBDA_H
#define LAMBDA_H

#include <monlang-LV2/ast/expr/Lambda.h>

#include <monlang-LV2/common.h>
#include <monlang-LV2/Statement.h>

#include <monlang-LV1/ast/Word.h>

template <>
struct MayFail_<LambdaBlock> {
    std::vector<MayFail<Statement_>> statements;
};

template <>
struct MayFail_<Lambda> {
    std::vector<identifier_t> parameters;
    MayFail_<LambdaBlock> body;
};

bool peekLambda(const Word&);

MayFail<MayFail_<Lambda>> buildLambda(const Word&);

template <>
LambdaBlock unwrap(const MayFail_<LambdaBlock>&);

template <>
MayFail_<LambdaBlock> wrap(const LambdaBlock&);

#endif // LAMBDA_H
