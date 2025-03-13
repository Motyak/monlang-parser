#ifndef NUMERAL_H
#define NUMERAL_H

#include <monlang-LV2/ast/expr/Numeral.h>

#include <monlang-LV1/ast/Word.h>

bool peekNumeral(const Word&);

Numeral buildNumeral(const Word&);

#endif // NUMERAL_H
