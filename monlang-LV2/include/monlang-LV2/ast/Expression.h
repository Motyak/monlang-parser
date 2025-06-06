#ifndef AST_EXPRESSION_H
#define AST_EXPRESSION_H

#include <variant>

struct Operation;
struct FunctionCall;
struct Lambda;
struct BlockExpression;
struct FieldAccess;
struct Subscript;
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
    FieldAccess*,
    Subscript*,

    /* SBG expressions */
    MapLiteral*,
    ListLiteral*,

    StrLiteral*,

    /* Atom expressions */
    Numeral*,
    SpecialSymbol*,
    Symbol*
>;

bool is_lvalue(Expression);

#endif // AST_EXPRESSION_H
