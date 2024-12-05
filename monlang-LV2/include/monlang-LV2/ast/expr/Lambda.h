#ifndef AST_LAMBDA_H
#define AST_LAMBDA_H

#include <monlang-LV2/ast/Statement.h>

#include <vector>
#include <string>

using identifier_t = std::string;

struct LambdaBlock {
    std::vector<Statement> statements;
};

struct Lambda {
    std::vector<identifier_t> parameters;
    LambdaBlock body;
};

#endif // AST_LAMBDA_H
