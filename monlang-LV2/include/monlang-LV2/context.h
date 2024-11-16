#ifndef CONTEXT_H
#define CONTEXT_H

#include <monlang-LV1/ProgramSentence.h>
#include <monlang-LV1/Term.h>

#include <any>

struct context_t {
    std::any statements; // pre-allocated std::vector<Statement>*
    ProgramSentence& sentence;
    Term& term;

    bool& fallthrough; // false initialized

    context_t();
};

#endif // CONTEXT_H
