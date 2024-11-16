#include <monlang-LV2/context.h>
#include <monlang-LV2/Statement.h>

#include <utils/mem-utils.h>

context_t::context_t() :
        statements(move_to_heap(std::vector<Statement>())),
        sentence(),
        term(),
        fallthrough(false){}
