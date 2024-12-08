#ifndef LITERAL_H
#define LITERAL_H

#include <monlang-LV2/ast/expr/Literal.h>

#include <monlang-LV1/ast/Word.h>

bool peekLiteral(const Word&);

Literal buildLiteral(const Word&);

#endif // LITERAL_H
