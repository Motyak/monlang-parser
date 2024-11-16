#ifndef RVALUE_H
#define RVALUE_H

#include <monlang-LV2/context.h>

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

Rvalue buildRvalue(const Term&, const context_t& = context_t{});

#endif // RVALUE_H
