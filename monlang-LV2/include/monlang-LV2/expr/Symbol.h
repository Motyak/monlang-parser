#ifndef SYMBOL_H
#define SYMBOL_H

#include <monlang-LV2/ast/expr/Symbol.h>

#include <monlang-LV1/ast/Word.h>

bool peekSymbol(const Word&);

Symbol buildSymbol(const Word&);

#endif // SYMBOL_H
