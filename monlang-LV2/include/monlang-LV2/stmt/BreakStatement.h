#ifndef BREAK_STATEMENT_H
#define BREAK_STATEMENT_H

#include <monlang-LV2/ast/stmt/BreakStatement.h>

#include <monlang-LV1/ast/Program.h>

bool peekBreakStatement(const ProgramSentence&);

BreakStatement* consumeBreakStatement(LV1::Program&);

#endif // BREAK_STATEMENT_H
