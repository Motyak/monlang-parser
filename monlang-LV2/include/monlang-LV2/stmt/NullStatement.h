#ifndef NULL_STMT_H
#define NULL_STMT_H

#include <monlang-LV2/ast/stmt/NullStatement.h>

#include <monlang-LV1/ast/Program.h>

bool peekNullStatement(const ProgramSentence&);

NullStatement* consumeNullStatement(LV1::Program&);

#endif // NULL_STMT_H
