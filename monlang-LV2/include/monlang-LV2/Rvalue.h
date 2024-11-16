#ifndef RVALUE_H
#define RVALUE_H

#include <monlang-LV1/Term.h>

#include <variant>

struct Operation;
struct FunctionCall;
struct Lambda;
struct BlockExpression;
struct Literal;
struct Lvalue;

using Rvalue = std::variant<
    Operation*,
    FunctionCall*,
    Lambda*,
    BlockExpression*,
    Literal*,
    Lvalue*
>;

Rvalue buildRvalue(const Term&);

#endif // RVALUE_H
