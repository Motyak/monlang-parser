#include <monlang-LV2/expr/Numeral.h>

/* impl only */
#include <monlang-LV1/ast/Atom.h>

#include <utils/assert-utils.h>

#define unless(x) if(!(x))

bool peekNumeral(const Word& word) {
    unless (std::holds_alternative<Atom*>(word)) {
        return false;
    }

    auto atom = *std::get<Atom*>(word);
    for (auto c: atom.value) {
        unless ('0' <= c && c <= '9') {
            return false;
        }
    }

    return true;
}

Numeral buildNumeral(const Word& word) {
    ASSERT (std::holds_alternative<Atom*>(word));
    auto atom = *std::get<Atom*>(word);

    auto numeral = Numeral{atom.value};
    numeral._tokenLen = atom._tokenLen;
    return numeral;
}

Numeral::Numeral(const std::string& str) : str(str){}
