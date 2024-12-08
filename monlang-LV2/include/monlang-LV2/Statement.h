#ifndef STATEMENT_H
#define STATEMENT_H

#include <monlang-LV2/ast/Statement.h>

#include <monlang-LV2/common.h>
#include <monlang-LV1/ast/Program.h>

using Statement_ = std::variant<
    /* assignments */
    MayFail_<Assignment>*, // a := b
    MayFail_<Accumulation>*, // +=, -=, *=, ^=, /=, %=, &=, |=

    /// LANGUAGE DEFINED STATEMENTS ///////////////////////

    /* declarations (introduce new symbol/identifier) */
    MayFail_<LetStatement>*, // let a b
    MayFail_<VarStatement>*, // var a b

    /* jump statements */
    MayFail_<ReturnStatement>*,
    BreakStatement*,
    ContinueStatement*,
    DieStatement*,

    // /* conditional statements */
    // MayFail_<Guard>*, // [ <cond> ] || <jump-when-fails>
    // MayFail_<IfStatement>*, // if..elsif..else, unless

    /* loop statements */
    MayFail_<ForeachStatement>*, // foreach <iterable> <block>
    // MayFail_<WhileStatement>*, // while, do..while, until, do..until

    // MayFail_<DeferStatement>*, // defer <block-expression>

    ///////////////////////////////////////////////////////

    // fall-through statement
    MayFail_<ExpressionStatement>*
>;

MayFail<Statement_> consumeStatement(LV1::Program&);

Statement unwrap_stmt(Statement_);
Statement_ wrap_stmt(Statement);

#endif // STATEMENT_H
