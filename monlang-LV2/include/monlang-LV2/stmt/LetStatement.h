#ifndef LET_STATEMENT_H
#define LET_STATEMENT_H

#include <monlang-LV2/Expression.h>
#include <monlang-LV2/context_init.h>

#include <monlang-LV1/ast/ProgramSentence.h>

using identifier_t = std::string;

struct LetStatement {
    identifier_t lhs;
    Expression rhs;
};

bool peekLetStatement(const ProgramSentence&);

LetStatement buildLetStatement(const ProgramSentence&, const context_t& = context_init_t{});

#endif // LET_STATEMENT_H
