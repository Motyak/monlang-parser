#include <monlang-LV2/context.h>
#include <monlang-LV2/Statement.h>
#include <monlang-LV2/context_init.h>

#include <monlang-LV1/ProgramSentence.h>
#include <monlang-LV1/Term.h>

#include <utils/mem-utils.h>

context_t::context_t() :
        disposable(false),
        statements(move_to_heap(std::vector<Statement>())),
        sentence(*move_to_heap(ProgramSentence())),
        term(*move_to_heap(Term())),
        malformed_stmt(*move_to_heap(std::optional<std::string>())),
        fallthrough(*move_to_heap(false)){}

context_t::context_t(bool disposable, std::any statements, ProgramSentence& sentence, Term& term, std::optional<std::string>& malformed_stmt, bool& fallthrough) :
        disposable(disposable),
        statements(statements),
        sentence(sentence),
        term(term),
        malformed_stmt(malformed_stmt),
        fallthrough(fallthrough){}

namespace
{
    struct disposable_context_t : public context_t {
        // will allocate memory and pass ownership
        disposable_context_t() : context_t(
            true,
            /* stub values from now on */
            move_to_heap(std::vector<Statement>()),
            *move_to_heap(ProgramSentence()),
            *move_to_heap(Term()),
            *move_to_heap(std::optional<std::string>()),
            *move_to_heap(bool())
        ){}
    };
}

const context_t* disposable_cx() {
    static thread_local context_t* cx = nullptr;
    if (cx) {
        delete cx;
    }
    cx = new disposable_context_t{};
    return cx;
}
