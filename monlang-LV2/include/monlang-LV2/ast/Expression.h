#ifndef AST_EXPRESSION_H
#define AST_EXPRESSION_H

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

    /// LANGUAGE DEFINED EXPRESSIONS //////////////////////

    FunctionCall*,
    Lambda*,
    BlockExpression*,
    Literal*,
    SpecialSymbol*,

    ///////////////////////////////////////////////////////

    // fall-through expression
    Lvalue*
>;

#endif // AST_EXPRESSION_H
