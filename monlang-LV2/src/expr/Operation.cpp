#include <monlang-LV2/expr/Operation.h>

/* impl only */

#include <monlang-LV1/ast/Atom.h>

#include <utils/assert-utils.h>

bool peekOperation(const Term& term) {
    return term.words.size() == 3;
}

MayFail<MayFail_<Operation>> buildOperation(const Term& term) {
    ASSERT (term.words.size() == 3);

    ASSERT (std::holds_alternative<Atom*>(term.words[1]));
    auto operator_ = std::get<Atom*>(term.words[1])->value;

    auto leftOperand = buildExpression(Term{{term.words[0]}});
    if (leftOperand.has_error()) {
        return Malformed(MayFail_<Operation>{Expression_(), operator_, Expression_()}, ERR(611));
    }

    auto rightOperand = buildExpression(Term{{term.words[2]}});
    if (rightOperand.has_error()) {
        return Malformed(MayFail_<Operation>{leftOperand, operator_, Expression_()}, ERR(612));
    }

    return MayFail_<Operation>{leftOperand, operator_, rightOperand};
}

template <>
Operation unwrap(const MayFail_<Operation>& operation) {
    auto leftOperand = unwrap_expr(operation.leftOperand.value());
    auto rightOperand = unwrap_expr(operation.rightOperand.value());

    return Operation{leftOperand, operation.operator_, rightOperand};
}

template <>
MayFail_<Operation> wrap(const Operation& operation) {
    auto leftOperand = wrap_expr(operation.leftOperand);
    auto rightOperand = wrap_expr(operation.rightOperand);

    return MayFail_<Operation>{leftOperand, operation.operator_, rightOperand};
}
