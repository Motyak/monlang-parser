#ifndef AST_STATEMENT_H
#define AST_STATEMENT_H

#include <variant>

struct Assignment;
struct Accumulation;
struct LetStatement;
struct VarStatement;
struct ReturnStatement;
struct BreakStatement;
struct ContinueStatement;
struct DieStatement;
struct Guard;
// struct IfStatement;
struct ForeachStatement;
struct WhileStatement;
struct DoWhileStatement;
// struct DeferStatement;
struct ExpressionStatement;

using Statement = std::variant<
    /* assignments */
    Assignment*, // a := b
    Accumulation*, // +=, -=, *=, ^=, /=, %=, &=, |=

    /// LANGUAGE DEFINED STATEMENTS ///////////////////////

    /* declarations (introduce new symbol/identifier) */
    LetStatement*, // let a b
    VarStatement*, // var a b

    /* jump statements */
    ReturnStatement*,
    BreakStatement*,
    ContinueStatement*,
    DieStatement*,

    // /* conditional statements */
    // Guard*, // [ <cond> ] || <jump-when-fails>
    // IfStatement*, // if..elsif..else, unless

    /* loop statements */
    ForeachStatement*, // foreach <iterable> <block>
    WhileStatement*, // while, until
    DoWhileStatement*, // do..while, do..until

    // DeferStatement*, // defer <block-expression>

    ///////////////////////////////////////////////////////

    // fall-through statement
    ExpressionStatement*
>;

#endif // AST_STATEMENT_H
