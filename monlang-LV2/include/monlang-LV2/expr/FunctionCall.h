#ifndef FUNCTION_CALL_H
#define FUNCTION_CALL_H

#include <monlang-LV2/ast/expr/FunctionCall.h>

#include <monlang-LV2/common.h>
#include <monlang-LV2/Expression.h>

#include <monlang-LV1/ast/Word.h>

template <>
struct MayFail_<FunctionCall> {
    MayFail<Expression_> function;
    std::vector<MayFail<Expression_>> arguments;
};

bool peekFunctionCall(const Word&);

MayFail<MayFail_<FunctionCall>> buildFunctionCall(const Word&);

template <>
FunctionCall unwrap(const MayFail_<FunctionCall>&);

template <>
MayFail_<FunctionCall> wrap(const FunctionCall&);

#endif // FUNCTION_CALL_H
