#ifndef VAR_STATEMENT_H
#define VAR_STATEMENT_H

#include <monlang-LV2/Expression.h>
#include <monlang-LV2/context.h>

#include <monlang-LV1/ast/ProgramSentence.h>

using identifier_t = std::string;

struct VarStatement {
    identifier_t identifier;
    Expression value;
};

bool peekVarStatement(const ProgramSentence&);

VarStatement buildVarStatement(const ProgramSentence&, context_t* = new context_t{});

#endif // VAR_STATEMENT_H
