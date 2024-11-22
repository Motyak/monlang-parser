#ifndef LVALUE_H
#define LVALUE_H

#include <monlang-LV1/ast/Term.h>

#include <string>

struct Lvalue {
    std::string identifier;
};

bool peekLvalue(const Term&);

Lvalue buildLvalue(const Term&);

#endif // LVALUE_H
