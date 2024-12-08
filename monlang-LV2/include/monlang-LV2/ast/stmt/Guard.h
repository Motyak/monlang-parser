#ifndef AST_GUARD_H
#define AST_GUARD_H

#include <monlang-LV2/ast/Expression.h>

struct ReturnStatement;
struct BreakStatement;
struct ContinueStatement;
struct DieStatement;

#define JUMP_STATEMENTS ReturnStatement*, BreakStatement*, ContinueStatement*, DieStatement*

struct Guard {
    Expression condition;
    std::variant<JUMP_STATEMENTS> jump;
};

#endif // AST_GUARD_H
