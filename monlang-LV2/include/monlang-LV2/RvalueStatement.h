#ifndef RVALUE_STATEMENT
#define RVALUE_STATEMENT

#include <monlang-LV2/Rvalue.h>

#include <monlang-LV1/ProgramSentence.h>

struct RvalueStatement {
    Rvalue rvalue;
};

RvalueStatement buildRvalueStatement(const ProgramSentence&);

#endif