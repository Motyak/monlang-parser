#include <monlang-LV2/Rvalue.h>

#include <utils/assert-utils.h>

Rvalue buildRvalue(const Term& term, const context_t& cx) {
    auto& fallthrough = cx.fallthrough;
    ASSERT (term.words.size() > 0);
    ASSERT (fallthrough == false); //

    // if (term =~ "RVALUE (BINARY-OPERATOR)*"_) {
    //     return buildOperation(term);
    // }

    // ...

    /* reached fall-through */
    fallthrough = true;
    return Rvalue(); // return stub
}
