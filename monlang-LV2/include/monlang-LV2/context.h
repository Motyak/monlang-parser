#ifndef CONTEXT_H
#define CONTEXT_H

struct ProgramSentence;
struct Term;

#include <any>
#include <optional>
#include <string>

struct context_t {
    const bool disposable;
    std::any statements; // pre-allocated std::vector<Statement>*
    ProgramSentence& sentence;
    Term& term;

    std::optional<std::string>& malformed_stmt;
    bool& fallthrough; // false initialized

    // will allocate memory and pass ownership
    context_t();
    // will store references to existing context_init_t
    context_t(bool, std::any, ProgramSentence&, Term&, std::optional<std::string>&, bool&);
};

const context_t* disposable_cx();

#endif // CONTEXT_H
