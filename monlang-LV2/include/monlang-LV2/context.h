#ifndef CONTEXT_H
#define CONTEXT_H

#include <monlang-LV2/Statement.h>

#include <monlang-LV1/ProgramSentence.h>
#include <monlang-LV1/Term.h>

#include <vector>

struct context_t {
    std::vector<Statement> statements;
    ProgramSentence sentence;
    Term term;

    bool fallthrough = false;
};

#endif // CONTEXT_H
