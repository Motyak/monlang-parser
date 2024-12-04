#ifndef SPECIAL_SYMBOL_H
#define SPECIAL_SYMBOL_H

#include <monlang-LV1/ast/Word.h>

#include <string>

struct SpecialSymbol {
    std::string str;
};

bool peekSpecialSymbol(const Word&);

SpecialSymbol buildSpecialSymbol(const Word&);

#endif // SPECIAL_SYMBOL_H
