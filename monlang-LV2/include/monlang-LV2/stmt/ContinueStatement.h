#ifndef CONTINUE_STATEMENT_H
#define CONTINUE_STATEMENT_H

#include <monlang-LV2/ast/stmt/ContinueStatement.h>

#include <monlang-LV1/ast/Program.h>

bool peekContinueStatement(const ProgramSentence&);

ContinueStatement* consumeContinueStatement(LV1::Program&);

#endif // CONTINUE_STATEMENT_H
