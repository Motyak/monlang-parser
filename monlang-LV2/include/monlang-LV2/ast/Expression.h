#ifndef AST_EXPRESSION_H
#define AST_EXPRESSION_H

#include <monlang-LV1/ast/Term.h>

#include <variant>

struct Operation;
struct FunctionCall;
struct Lambda;
struct BlockExpression;
struct Numeral;
struct StrLiteral;
struct SpecialSymbol;
struct Symbol;

using Expression = std::variant<
    /* Term expressions */

    Operation*,

    /* Word expressions */

    FunctionCall*,
    Lambda*,
    BlockExpression*,

    /* Atom expressions */

    Numeral*,
    StrLiteral*,
    SpecialSymbol*,
    // fall-through
    Symbol*
>;

bool is_lvalue(Expression);

#endif // AST_EXPRESSION_H
