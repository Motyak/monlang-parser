#ifndef CONTEXT_INIT_H
#define CONTEXT_INIT_H

#include <monlang-LV2/Statement.h>
#include <monlang-LV2/context.h>

#include <monlang-LV1/ast/ProgramSentence.h>
#include <monlang-LV1/ast/Term.h>

struct context_init_t {
    std::vector<Statement> statements;
    ProgramSentence sentence;
    Term term;

    bool fallthrough; // false initialized

    context_init_t();
    operator context_t();
};

#endif // CONTEXT_INIT_H
