#include <monlang-LV2/expr/Operation.h>

/* impl only */

#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/ParenthesesGroup.h>
#include <monlang-LV1/ast/common.h>

#include <utils/assert-utils.h>

bool peekOperation(const Term& term) {
    return term.words.size() == 3;
}

MayFail<MayFail_<Operation>> buildOperation(const Term& term, std::stack<Alteration>* alterations) {
    ASSERT (term.words.size() == 3);
    ASSERT (alterations != nullptr);
    ASSERT (!alterations->empty());

    ASSERT (std::holds_alternative<Atom*>(term.words[1]));
    auto operator_ = std::get<Atom*>(term.words[1])->value;

    auto leftOperand = buildExpression(Term{{term.words[0]}});
    if (leftOperand.has_error()) {
        return Malformed(MayFail_<Operation>{Expression_(), operator_, Expression_()}, ERR(611));
    }

    auto rightOperand = buildExpression(Term{{term.words[2]}});
    if (rightOperand.has_error()) {
        return Malformed(MayFail_<Operation>{leftOperand, operator_, rightOperand}, ERR(612));
    }

    /* fix operands' _tokenLen by popping alterations stack */
    if (alterations->top() == Alteration::LEFT_OPND) {
        leftOperand.val._tokenLen -= sequenceLen(ParenthesesGroup::INITIATOR_SEQUENCE)
                + sequenceLen(ParenthesesGroup::TERMINATOR_SEQUENCE);
    }
    else if (alterations->top() == Alteration::RIGHT_OPND) {
        rightOperand.val._tokenLen -= sequenceLen(ParenthesesGroup::INITIATOR_SEQUENCE)
                + sequenceLen(ParenthesesGroup::TERMINATOR_SEQUENCE);
    }
    else /* ::NONE */ {
        ; // do nothing
    }
    alterations->pop();

    // TODO: setter _tokenLen NON PAS par rapport à term._tokenLen (créé par fixPrecedence())..
    //       ..MAIS en faisant la somme des opérandes + opérateur + espaces ? WAAAAAAH
    return MayFail_<Operation>{leftOperand, operator_, rightOperand};
}

MayFail_<Operation>::MayFail_(MayFail<Expression_> leftOperand, identifier_t operator_, MayFail<Expression_> rightOperand)
        : leftOperand(leftOperand), operator_(operator_), rightOperand(rightOperand){}

MayFail_<Operation>::MayFail_(Operation operation) {
    this->leftOperand = wrap_expr(operation.leftOperand);
    this->operator_ = operation.operator_;
    this->rightOperand = wrap_expr(operation.rightOperand);
}

MayFail_<Operation>::operator Operation() const {
    auto leftOperand = unwrap_expr(this->leftOperand.value());
    auto rightOperand = unwrap_expr(this->rightOperand.value());
    return Operation{leftOperand, this->operator_, rightOperand};
}
