#ifndef STATEMENT_H
#define STATEMENT_H

#include <monlang-LV2/context.h>

#include <monlang-LV1/ast/Program.h>

#include <variant>

// struct Assignment;
// struct Accumulation;
// struct LetStatement;
// struct VarStatement;
// struct Guard;
// struct IfStatement;
// struct ForeachStatement;
// struct WhileStatement;
// struct ReturnStatement;
// struct BreakStatement;
// struct ContinueStatement;
// struct DieStatement;
struct RvalueStatement;

using Statement = std::variant<RvalueStatement*>;

// using Statement = std::variant<
//     /* assignments */
//     Assignment*, // a := b
//     Accumulation*, // +=, -=, *=, /=, %=, &=, |=, ^=, <<=, >>=

//     /* declarations */
//     LetStatement*, // let a b
//     VarStatement*, // var a b

//     /* conditional statements */
//     Guard*, // [ <cond> ] || <jump-when-fails>
//     IfStatement*, // if..elsif..else, unless

//     /* loop statements */
//     ForeachStatement*, // foreach <iterable> <block>
//     WhileStatement*, // while, do..while, until, do..until

//     /* jump statements */
//     ReturnStatement*,
//     BreakStatement*,
//     ContinueStatement*,
//     DieStatement*,

//     // fall-through statement
//     RvalueStatement* // expression on a single program sentence
// >;

Statement consumeStatement(LV1::Program&, const context_t& = context_t{});

#endif // STATEMENT_H
