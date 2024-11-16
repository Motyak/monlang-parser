#ifndef RVALUE_H
#define RVALUE_H

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

#endif // RVALUE_H
