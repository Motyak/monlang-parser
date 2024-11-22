#include <monlang-LV2/Lvalue.h>

/* impl only */
#include <monlang-LV1/ast/Atom.h>

#include <utils/assert-utils.h>

bool peekLvalue(const Term& term) {
    return term.words.size() == 1
            && std::holds_alternative<Atom*>(term.words[0]);
}

Lvalue buildLvalue(const Term& term) {
    ASSERT (term.words.size() == 1);
    ASSERT (std::holds_alternative<Atom*>(term.words[0]));

    auto atom = *std::get<Atom*>(term.words[0]);
    return Lvalue{atom.value};
}
