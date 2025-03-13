#ifndef STR_LITERAL_H
#define STR_LITERAL_H

#include <monlang-LV2/ast/expr/StrLiteral.h>

#include <monlang-LV1/ast/Word.h>

bool peekStrLiteral(const Word&);

StrLiteral buildStrLiteral(const Word&);

#endif // STR_LITERAL_H
