#ifndef CONTEXT_H
#define CONTEXT_H

struct ProgramSentence;
struct Term;

#include <any>
#include <optional>
#include <string>

struct context_t {
    std::any statements; // pre-allocated std::vector<Statement>*
    ProgramSentence& sentence;
    Term& term;

    std::optional<std::string>& malformed_stmt;
    bool& fallthrough; // false initialized

    context_t();
    context_t(std::any, ProgramSentence&, Term&, std::optional<std::string>&, bool&);
};

#endif // CONTEXT_H
