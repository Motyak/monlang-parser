#ifndef FUNCTION_CALL_H
#define FUNCTION_CALL_H

#include <monlang-LV2/Rvalue.h>

struct FunctionCall {
    Rvalue function;
    std::vector<Rvalue> arguments;
};

bool peekFunctionCall(const Word&);

FunctionCall buildFunctionCall(const Word&, const context_t& = context_t{});

#endif // FUNCTION_CALL_H
