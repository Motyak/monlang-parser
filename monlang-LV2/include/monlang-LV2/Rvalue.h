#ifndef RVALUE_H
#define RVALUE_H

#include <monlang-LV2/context.h>

#include <monlang-LV1/ast/Term.h>

#include <variant>

// struct Operation;
// struct FunctionCall;
struct Lambda;
struct BlockRvalue;
struct Literal;
struct Lvalue;

using Rvalue = std::variant<
    // Operation*,
    // FunctionCall*,
    Lambda*,
    BlockRvalue*,
    Literal*,
    Lvalue*
>;

Rvalue buildRvalue(const Term&, const context_t& = context_t{});

#endif // RVALUE_H
