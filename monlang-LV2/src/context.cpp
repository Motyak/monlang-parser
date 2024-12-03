#include <monlang-LV2/context.h>
#include <monlang-LV2/Statement.h>

#include <monlang-LV1/ProgramSentence.h>
#include <monlang-LV1/Term.h>

#include <utils/mem-utils.h>

context_t::context_t() :
        statements(move_to_heap(std::vector<Statement>())),
        sentence(move_to_heap(ProgramSentence())),
        term(move_to_heap(Term())),
        malformed_stmt(move_to_heap(std::optional<std::string>())),
        fallthrough(move_to_heap(false)){}

context_t::context_t(std::any statements, ProgramSentence* sentence, Term* term, std::optional<std::string>* malformed_stmt, bool* fallthrough) :
        statements(statements),
        sentence(sentence),
        term(term),
        malformed_stmt(malformed_stmt),
        fallthrough(fallthrough){}
