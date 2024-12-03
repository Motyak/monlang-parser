#ifndef STATEMENT_H
#define STATEMENT_H

#include <monlang-LV2/context.h>

#include <monlang-LV1/ast/Program.h>

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
// struct WhileStatement;
// struct DeferStatement;
struct ExpressionStatement;

using Statement = std::variant<
    /* declarations (introduce new symbol/identifier) */
    LetStatement*, // let a b
    VarStatement*, // var a b

    /* assignments */
    Assignment*, // a := b
    Accumulation*, // +=, -=, *=, ^=, /=, %=, &=, |=

    /* jump statements */
    ReturnStatement*,
    BreakStatement*,
    ContinueStatement*,
    DieStatement*,

    // /* conditional statements */
    // Guard*, // [ <cond> ] || <jump-when-fails>
    // IfStatement*, // if..elsif..else, unless

    // /* loop statements */
    ForeachStatement*, // foreach <iterable> <block>
    // WhileStatement*, // while, do..while, until, do..until

    // DeferStatement*, // defer <block-expression>

    // fall-through statement
    ExpressionStatement*
>;

Statement consumeStatement(LV1::Program&, context_t* = new context_t{});

#endif // STATEMENT_H
