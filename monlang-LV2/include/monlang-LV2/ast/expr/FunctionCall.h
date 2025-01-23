#ifndef AST_FUNCTION_CALL_H
#define AST_FUNCTION_CALL_H

#include <monlang-LV2/ast/Expression.h>

#include <vector>

struct FunctionCall {
    Expression function;
    std::vector<Expression> arguments;

    size_t _tokenLen = 0;
    size_t _groupNesting = 0;
    FunctionCall() = default;
    FunctionCall(const Expression&, const std::vector<Expression>&);
};

#endif // AST_FUNCTION_CALL_H
