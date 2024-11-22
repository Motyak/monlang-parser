#include <monlang-LV2/Rvalue.h>

/* impl only */
#include <monlang-LV2/Lvalue.h>

#include <utils/assert-utils.h>
#include <utils/mem-utils.h>

Rvalue buildRvalue(const Term& term, const context_t& cx) {
    auto& fallthrough = cx.fallthrough;
    ASSERT (term.words.size() > 0);

    // if (term =~ "RVALUE (BINARY-OPERATOR)*"_) {
    //     return buildOperation(term);
    // }

    // ...

    if (peekLvalue(term)) {
        return move_to_heap(buildLvalue(term));
    }

    /* reached fall-through */
    fallthrough = true;
    return Rvalue(); // return stub
}
