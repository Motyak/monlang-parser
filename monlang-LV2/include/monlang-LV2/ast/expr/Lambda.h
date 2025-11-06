#ifndef AST_LAMBDA_H
#define AST_LAMBDA_H

#include <monlang-LV2/ast/expr/Symbol.h>
#include <monlang-LV2/ast/expr/BlockExpression.h>

#include <vector>
#include <string>
#include <optional>

using LambdaBlock = BlockExpression;

struct Lambda {
    std::vector<Symbol> parameters;
    std::optional<Symbol> variadicParameter;
    LambdaBlock body;
    size_t _tokenLen = 0;
    size_t _tokenId = 123456789;
    Lambda() = default;
    Lambda(const std::vector<Symbol>&, const std::optional<Symbol>&, const LambdaBlock&);
};

#endif // AST_LAMBDA_H
