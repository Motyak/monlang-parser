#ifndef LET_STATEMENT_H
#define LET_STATEMENT_H

#include <monlang-LV2/Expression.h>
#include <monlang-LV2/context.h>

#include <monlang-LV1/ast/ProgramSentence.h>

using identifier_t = std::string;

struct LetStatement {
    identifier_t identifier;
    Expression value;
};

bool peekLetStatement(const ProgramSentence&);

LetStatement buildLetStatement(const ProgramSentence&, context_t* = new context_t{});

#endif // LET_STATEMENT_H
