#ifndef AST_EXPRESSION_H
#define AST_EXPRESSION_H

#include <monlang-LV1/ast/Term.h>

#include <variant>

struct Operation;
struct FunctionCall;
struct Lambda;
struct BlockExpression;
struct MapLiteral;
struct ListLiteral;
struct StrLiteral;
struct Numeral;
struct SpecialSymbol;
struct Symbol;

using Expression = std::variant<
    /* Term expressions */

    Operation*,

    /* Word expressions */

    FunctionCall*,
    Lambda*,
    BlockExpression*,

    /* SBG expressions */
    MapLiteral*,
    ListLiteral*,

    StrLiteral*,

    /* Atom expressions */

    Numeral*,
    SpecialSymbol*,
    // fall-through
    Symbol*
>;

bool is_lvalue(Expression);

#endif // AST_EXPRESSION_H
