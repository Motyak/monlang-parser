#include <monlang-LV2/Rvalue.h>

/* impl only */
#include <monlang-LV2/Lvalue.h>

#include <monlang-LV1/ast/ParenthesesGroup.h>

#include <utils/assert-utils.h>
#include <utils/mem-utils.h>

Rvalue buildRvalue(const Term& term, const context_t& cx) {
    auto& fallthrough = cx.fallthrough;
    ASSERT (term.words.size() > 0);
    auto term_ = term; // local non-const working variable

    BEGIN:

    // if (term =~ "RVALUE (BINARY-OPERATOR)*"_) {
    //     return buildOperation(term);
    // }

    // ...

    if (peekLvalue(term_)) {
        return move_to_heap(buildLvalue(term_));
    }

    // if grouped rvalue => unwrap then go back to beginning
    if (term_.words.size() == 1 && std::holds_alternative<ParenthesesGroup*>(term_.words[0])) {
        auto group = *std::get<ParenthesesGroup*>(term_.words[0]);
        if (group.terms.size() == 1) {
            term_ = group.terms[0];
            goto BEGIN;
        }
    }

    /* reached fall-through */
    fallthrough = true;
    return Rvalue(); // return stub
}
