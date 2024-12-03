#ifndef FUNCTION_CALL_H
#define FUNCTION_CALL_H

#include <monlang-LV2/Expression.h>

struct FunctionCall {
    Expression function;
    std::vector<Expression> arguments;
};

bool peekFunctionCall(const Word&);

FunctionCall buildFunctionCall(const Word&, context_t* = new context_t{});

#endif // FUNCTION_CALL_H
