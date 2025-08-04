#ifndef PRECEDENCE_H
#define PRECEDENCE_H

#include <monlang-LV1/ast/Term.h>

enum Associativity {
    LEFT_ASSOCIATIVE,
    RIGHT_ASSOCIATIVE,
    NOT_REALLY = LEFT_ASSOCIATIVE,
};

using Operators = std::vector<std::string>;
extern const std::vector<std::pair<Operators,Associativity>>
PRECEDENCE_TABLE;
extern const Operators
COMPOUND_ASSIGNMENT;


void fixPrecedence(Term&);

#endif // PRECEDENCE_H
