#include <monlang-LV2/expr/Symbol.h>

/* impl only */
#include <monlang-LV1/ast/Atom.h>

#include <utils/assert-utils.h>

bool peekSymbol(const Word& word) {
    return std::holds_alternative<Atom*>(word);
}

Symbol buildSymbol(const Word& word) {
    ASSERT (std::holds_alternative<Atom*>(word));

    auto atom = *std::get<Atom*>(word);
    auto symbol = Symbol{atom.value};
    symbol._tokenLen = atom._tokenLen;
    return symbol;
}

Symbol::Symbol(const std::string& value) : value(value){}
