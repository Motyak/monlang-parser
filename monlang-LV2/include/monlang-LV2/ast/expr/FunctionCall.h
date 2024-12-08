#ifndef AST_FUNCTION_CALL_H
#define AST_FUNCTION_CALL_H

#include <monlang-LV2/ast/Expression.h>

#include <vector>

struct FunctionCall {
    Expression function;
    std::vector<Expression> arguments;
};

#endif // AST_FUNCTION_CALL_H
