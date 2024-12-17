#include <monlang-LV2/expr/Lvalue.h>

/* impl only */
#include <monlang-LV1/ast/Atom.h>

#include <utils/assert-utils.h>

bool peekLvalue(const Word& word) {
    return std::holds_alternative<Atom*>(word);
}

Lvalue buildLvalue(const Word& word) {
    ASSERT (std::holds_alternative<Atom*>(word));

    auto atom = *std::get<Atom*>(word);
    auto lvalue = Lvalue{atom.value};
    lvalue._tokenLen = atom._tokenLen;
    return lvalue;
}

Lvalue::Lvalue(const std::string& identifier) : identifier(identifier){}
