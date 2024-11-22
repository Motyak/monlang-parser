#ifndef RVALUE_STATEMENT
#define RVALUE_STATEMENT

#include <monlang-LV2/Rvalue.h>
#include <monlang-LV2/context.h>

#include <monlang-LV1/ast/ProgramSentence.h>

struct RvalueStatement {
    Rvalue rvalue;
};

RvalueStatement buildRvalueStatement(const ProgramSentence&, const context_t& = context_t{});

#endif
