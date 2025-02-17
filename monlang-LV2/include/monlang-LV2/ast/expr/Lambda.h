#ifndef AST_LAMBDA_H
#define AST_LAMBDA_H

#include <monlang-LV2/ast/expr/Symbol.h>
#include <monlang-LV2/ast/expr/BlockExpression.h>

#include <vector>
#include <string>

using LambdaBlock = BlockExpression;

struct Lambda {
    std::vector<Symbol> parameters;
    LambdaBlock body;

    size_t _tokenLen = 0;
    size_t _groupNesting = 0;
    Lambda() = default;
    Lambda(const std::vector<Symbol>&, const LambdaBlock&);
};

#endif // AST_LAMBDA_H
