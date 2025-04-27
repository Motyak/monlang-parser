#ifndef AST_WHILE_STATEMENT_H
#define AST_WHILE_STATEMENT_H

#include <monlang-LV2/ast/Expression.h>
#include <monlang-LV2/ast/expr/BlockExpression.h>

#include <string>
#include <cstddef>

using WhileBlock = BlockExpression;

struct WhileStatement {
    static const std::string WHILE_KEYWORD;
    static const std::string UNTIL_KEYWORD;

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

/* compound statements */
struct C_DoStatement;
struct C_WhileStatement;

struct C_DoStatement {
    static const std::string KEYWORD;

    WhileBlock block;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    C_DoStatement() = default;
    C_DoStatement(const WhileBlock&);
};

struct C_WhileStatement {
    static const std::string WHILE_KEYWORD;
    static const std::string UNTIL_KEYWORD;

    Expression condition;
    bool until_loop = false;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    C_WhileStatement() = default;
    C_WhileStatement(const Expression&, bool);
};

struct DoWhileStatement {
    C_DoStatement doStmt;
    C_WhileStatement whileStmt;
};

#endif // AST_WHILE_STATEMENT_H
