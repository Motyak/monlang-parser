#include <monlang-LV2/expr/SpecialSymbol.h>

/* impl only */
#include <monlang-LV1/ast/Atom.h>

#include <utils/assert-utils.h>

#define unless(x) if(!(x))

bool peekSpecialSymbol(const Word& word) {
    unless (std::holds_alternative<Atom*>(word)) {
        return false;
    }

    auto atom = *std::get<Atom*>(word);
    unless (atom.value.size() >= 1) {
        return false;
    }

    return atom.value[0] == '$';
}

SpecialSymbol buildSpecialSymbol(const Word& word) {
    ASSERT (std::holds_alternative<Atom*>(word));
    auto atom = *std::get<Atom*>(word);

    return SpecialSymbol{atom.value};
}
