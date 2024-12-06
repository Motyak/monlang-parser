#ifndef AST_LAMBDA_H
#define AST_LAMBDA_H

#include <monlang-LV2/ast/expr/BlockExpression.h>

#include <vector>
#include <string>

using identifier_t = std::string;
using LambdaBlock = BlockExpression;

struct Lambda {
    std::vector<identifier_t> parameters;
    LambdaBlock body;
};

#endif // AST_LAMBDA_H
