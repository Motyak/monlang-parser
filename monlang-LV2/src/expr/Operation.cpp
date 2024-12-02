#include <monlang-LV2/expr/Operation.h>

#include <monlang-LV1/ast/Atom.h>

#include <utils/assert-utils.h>

bool peekOperation(const Term& term) {
    return term.words.size() == 3;
}

Operation buildOperation(const Term& term, context_t* cx) {
    auto& fallthrough = *cx->fallthrough;

    ASSERT (!fallthrough);
    ASSERT (term.words.size() == 3);

    auto leftOperand = buildExpression(Term{{term.words[0]}}, cx);
    if (fallthrough) {
        return Operation(); // stub
    }

    ASSERT (std::holds_alternative<Atom*>(term.words[1]));
    auto operator_ = std::get<Atom*>(term.words[1])->value;

    auto rightOperand = buildExpression(Term{{term.words[2]}}, cx);
    if (fallthrough) {
        return Operation(); // stub
    }

    return Operation{leftOperand, operator_, rightOperand};
}
