#ifndef LVALUE_H
#define LVALUE_H

#include <monlang-LV1/ast/Word.h>

#include <string>

struct Lvalue {
    std::string identifier;
};

bool peekLvalue(const Word&);

Lvalue buildLvalue(const Word&);

#endif // LVALUE_H
