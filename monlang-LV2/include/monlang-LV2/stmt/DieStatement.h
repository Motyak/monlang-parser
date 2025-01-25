#ifndef DIE_STATEMENT_H
#define DIE_STATEMENT_H

#include <monlang-LV2/ast/stmt/DieStatement.h>

#include <monlang-LV1/ast/Program.h>

bool peekDieStatement(const ProgramSentence&);

DieStatement* consumeDieStatement(LV1::Program&);

#endif // DIE_STATEMENT_H
