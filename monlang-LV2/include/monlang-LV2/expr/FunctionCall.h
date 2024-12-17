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

    size_t _tokenLen = 0;
    MayFail_() = default;
    explicit MayFail_(MayFail<Expression_>, std::vector<MayFail<Expression_>>);

    explicit MayFail_(FunctionCall);
    explicit operator FunctionCall() const;
};

bool peekFunctionCall(const Word&);

MayFail<MayFail_<FunctionCall>> buildFunctionCall(const Word&);

#endif // FUNCTION_CALL_H
