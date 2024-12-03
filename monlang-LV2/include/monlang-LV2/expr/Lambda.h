#ifndef LAMBDA_H
#define LAMBDA_H

#include <monlang-LV2/Statement.h>
#include <monlang-LV2/context.h>

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

bool peekLambda(const Word&);

Lambda buildLambda(const Word&, const context_t*);

#endif // LAMBDA_H
