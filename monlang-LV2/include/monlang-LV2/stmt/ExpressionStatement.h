#ifndef EXPRESSION_STATEMENT_H
#define EXPRESSION_STATEMENT_H

#include <monlang-LV2/Expression.h>
#include <monlang-LV2/context.h>

#include <monlang-LV1/ast/ProgramSentence.h>

struct ExpressionStatement {
    Expression expression;
};

ExpressionStatement buildExpressionStatement(const ProgramSentence&, const context_t* = fresh_cx());

#endif // EXPRESSION_STATEMENT_H
