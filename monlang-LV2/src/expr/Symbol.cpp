#include <monlang-LV2/expr/Symbol.h>

/* impl only */
#include <monlang-LV2/Expression.h>
#include <monlang-LV1/ast/Atom.h>

#include <utils/assert-utils.h>

/*
    in terms of performance, it's preferable to do:
    buildExpression && unless hold_alternative<Symbol> err
    rather than..
    unless peekSymbol err
    (e.g.: see consumeLetStatement identifier)
*/
bool peekSymbol(const Word& word) {
    if (!std::holds_alternative<Atom*>(word)) {
        return false;
    }
    // unfortunately the most flexible way is to call buildExpression
    // ..and check variant, otherwise would need to check that..
    // ..word doesn't match any other Atom expression (Literal, SpecialSymbol, etc..)
    auto expr_ = buildExpression((Term)word);
    auto res = std::holds_alternative<Symbol*>(expr_.val);

    // we can safely delete Expression_ pointer before returning
    delete_(expr_.val);

    return res;
}

Symbol buildSymbol(const Word& word) {
    ASSERT (std::holds_alternative<Atom*>(word));

    auto atom = *std::get<Atom*>(word);
    auto symbol = Symbol{atom.value};
    symbol._tokenLen = atom._tokenLen;
    return symbol;
}

Symbol::Symbol(const std::string& value) : value(value){}
