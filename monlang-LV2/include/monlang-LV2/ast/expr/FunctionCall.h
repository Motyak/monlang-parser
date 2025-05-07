#ifndef AST_FUNCTION_CALL_H
#define AST_FUNCTION_CALL_H

#include <monlang-LV2/ast/Expression.h>

#include <vector>

struct FunctionCall {
    struct Argument {
        Expression expr;
        bool passByRef = false;

        size_t _tokenLen = 0;
        Argument() = default;
        Argument(const Expression&, bool = false);
    };

    Expression function;
    std::vector<Argument> arguments;

    size_t _tokenLen = 0;
    FunctionCall() = default;
    FunctionCall(const Expression&, const std::vector<Argument>&);
};

#endif // AST_FUNCTION_CALL_H
