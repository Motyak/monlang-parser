#ifndef PRECEDENCE_H
#define PRECEDENCE_H

#include <monlang-LV1/ast/Term.h>

#include <stack>

enum Associativity {
    LEFT_ASSOCIATIVE,
    RIGHT_ASSOCIATIVE,
};

using Operators = std::vector<std::string>;
extern const std::vector<std::pair<Operators,Associativity>>
PRECEDENCE_TABLE;

void fixPrecedence(Term&);

enum class Alteration {
    LEFT_OPND,
    RIGHT_OPND,
    NONE
};
// overloaded function to accept a tracing object by reference
void fixPrecedence(Term&, std::stack<Alteration>&);

#endif // PRECEDENCE_H
