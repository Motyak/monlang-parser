#ifndef VAR_STATEMENT_H
#define VAR_STATEMENT_H

#include <monlang-LV2/Expression.h>
#include <monlang-LV2/context_init.h>

#include <monlang-LV1/ast/ProgramSentence.h>

using identifier_t = std::string;

struct VarStatement {
    identifier_t lhs;
    Expression rhs;
};

bool peekVarStatement(const ProgramSentence&);

VarStatement buildVarStatement(const ProgramSentence&, const context_t& = context_init_t{});

#endif // VAR_STATEMENT_H
