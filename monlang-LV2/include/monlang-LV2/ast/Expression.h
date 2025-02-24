#ifndef AST_EXPRESSION_H
#define AST_EXPRESSION_H

#include <monlang-LV1/ast/Term.h>

#include <variant>

struct Operation;
struct FunctionCall;
struct Lambda;
struct BlockExpression;
struct Literal;
struct SpecialSymbol;
struct Symbol;
struct Lvalue; // <- should disappear by then

using Expression = std::variant<
    /* Term expressions */

    Operation*,

    /* Word expressions */

    FunctionCall*,
    Lambda*,
    BlockExpression*,

    /* Atom expressions */

    Literal*,
    SpecialSymbol*,
    // fall-through
    Symbol*
>;

bool is_lvalue(Expression);

#endif // AST_EXPRESSION_H
