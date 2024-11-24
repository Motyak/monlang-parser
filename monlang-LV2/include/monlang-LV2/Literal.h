#ifndef LITERAL_H
#define LITERAL_H

#include <monlang-LV1/ast/Word.h>

#include <string>

struct Literal {
    std::string str;
};

bool peekLiteral(const Word&);

Literal buildLiteral(const Word&);

#endif // LITERAL_H
