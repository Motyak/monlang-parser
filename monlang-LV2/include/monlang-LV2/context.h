#ifndef CONTEXT_H
#define CONTEXT_H

struct ProgramSentence;
struct Term;

#include <any>

struct context_t {
    std::any statements; // pre-allocated std::vector<Statement>*
    ProgramSentence& sentence;
    Term& term;

    bool& fallthrough; // false initialized

    context_t();
    context_t(std::any, ProgramSentence&, Term&, bool&);
};

#endif // CONTEXT_H
