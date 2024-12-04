#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <monlang-LV2/context.h>

#include <monlang-LV1/ast/Term.h>

#include <variant>

struct Operation;
struct FunctionCall;
struct Lambda;
struct BlockExpression;
struct SpecialSymbol;
struct Literal;
struct Lvalue;

using Expression = std::variant<
    Operation*,
    FunctionCall*,
    Lambda*,
    BlockExpression*,
    Literal*,
    SpecialSymbol*,
    Lvalue*
>;

Expression buildExpression(const Term&, context_t* = new context_t{});

#endif // EXPRESSION_H
