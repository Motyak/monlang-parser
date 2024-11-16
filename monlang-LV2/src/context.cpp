#include <monlang-LV2/context.h>
#include <monlang-LV2/Statement.h>

#include <utils/mem-utils.h>

context_t::context_t() :
        statements(move_to_heap(std::vector<Statement>())),
        sentence(*move_to_heap(ProgramSentence())),
        term(*move_to_heap(Term())),
        fallthrough(*move_to_heap(false)){}
