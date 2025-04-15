#ifndef FUNCTION_CALL_H
#define FUNCTION_CALL_H

#include <monlang-LV2/ast/expr/FunctionCall.h>

#include <monlang-LV2/common.h>
#include <monlang-LV2/Expression.h>

#include <monlang-LV1/ast/Word.h>

template <>
struct MayFail_<FunctionCall> {
    struct Argument {
        MayFail<Expression_> expr;
        bool passByRef = false;

        size_t _tokenLen = 0;
        Argument() = default;
        Argument(const MayFail<Expression_>&, bool);
    };

    MayFail<Expression_> function;
    std::vector<MayFail<Argument>> arguments;

    size_t _tokenLen = 0;
    size_t _groupNesting = 0;
    MayFail_() = default;
    explicit MayFail_(const MayFail<Expression_>&, const std::vector<MayFail<Argument>>&);

    explicit MayFail_(FunctionCall);
    explicit operator FunctionCall() const;
};

bool peekFunctionCall(const Word&);

MayFail<MayFail_<FunctionCall>> buildFunctionCall(const Word&);

#endif // FUNCTION_CALL_H
