#ifndef STATEMENT_H
#define STATEMENT_H

#include <monlang-LV2/ast/Statement.h>

#include <monlang-LV2/common.h>
#include <monlang-LV1/ast/Program.h>

struct _StubStatement_ {
    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
};

using Statement_ = std::variant<

    _StubStatement_*, // to be used when..
    // ..the statement itself is malformed
    // ..(cannot be associated with any type of statement)

    ///////////////////////////////////////////////////////

    /* assignments */
    MayFail_<Assignment>*, // a := b
    MayFail_<Accumulation>*, // +=, -=, *=, ^=, /=, %=, &=, |=

    /// LANGUAGE DEFINED STATEMENTS ///////////////////////

    /* declarations (introduce new symbol in environment) */
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
    MayFail_<WhileStatement>*, // while, until
    MayFail_<DoWhileStatement>*, // do..while, do..until

    // MayFail_<DeferStatement>*, // defer <block-expression>

    NullStatement*, // ;

    ///////////////////////////////////////////////////////

    // fall-through statement
    MayFail_<ExpressionStatement>*
>;

MayFail<Statement_> consumeStatement(LV1::Program&);

Statement unwrap_stmt(Statement_);
Statement_ wrap_stmt(Statement);

template <typename T> size_t token_leading_newlines(const T& entity) {
    return entity._tokenLeadingNewlines;
}
template <typename T> size_t token_indent_spaces(const T& entity) {
    return entity._tokenIndentSpaces;
}
template <typename T> size_t token_trailing_newlines(const T& entity) {
    return entity._tokenTrailingNewlines;
}

#endif // STATEMENT_H
