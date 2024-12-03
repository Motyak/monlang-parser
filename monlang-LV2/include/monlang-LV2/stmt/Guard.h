#ifndef GUARD_H
#define GUARD_H

#include <monlang-LV2/context_init.h>

struct ReturnStatement;
struct BreakStatement;
struct ContinueStatement;
struct DieStatement;

#define JUMP_STATEMENTS ReturnStatement*, BreakStatement*, ContinueStatement*, DieStatement*

struct Guard {
    Expression condition;
    std::variant<JUMP_STATEMENTS> jump;
};

bool peekGuard(const ProgramSentence&);

Guard buildGuard(const ProgramSentence&, const context_t*);

#endif // GUARD_H
