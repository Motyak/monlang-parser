#ifndef AST_WHILE_STATEMENT_H
#define AST_WHILE_STATEMENT_H

#include <monlang-LV2/ast/Expression.h>
#include <monlang-LV2/ast/expr/BlockExpression.h>

using WhileBlock = BlockExpression;

struct WhileStatement {
    static const Atom WHILE_KEYWORD;
    static const Atom UNTIL_KEYWORD;

    Expression condition;
    WhileBlock block;
    bool until_loop = false;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    WhileStatement() = default;
    WhileStatement(const Expression&, const WhileBlock&, bool);
};

struct DoWhileStatement {
    static const Atom DO_KEYWORD;
    static const Atom WHILE_KEYWORD;
    static const Atom UNTIL_KEYWORD;

    WhileBlock block;
    Expression condition;
    bool until_loop = false;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    DoWhileStatement() = default;
    DoWhileStatement(const WhileBlock&, const Expression&, bool);
};

#endif // AST_WHILE_STATEMENT_H
