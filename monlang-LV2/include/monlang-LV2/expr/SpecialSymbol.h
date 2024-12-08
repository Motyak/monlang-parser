#ifndef SPECIAL_SYMBOL_H
#define SPECIAL_SYMBOL_H

#include <monlang-LV2/ast/expr/SpecialSymbol.h>

#include <monlang-LV1/ast/Word.h>

bool peekSpecialSymbol(const Word&);

SpecialSymbol buildSpecialSymbol(const Word&);

#endif // SPECIAL_SYMBOL_H
