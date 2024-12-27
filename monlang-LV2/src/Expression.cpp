#include <monlang-LV2/Expression.h>

/* impl only */
#include <monlang-LV2/precedence.h> // fixPrecedence()
#include <monlang-LV2/expr/Operation.h>
#include <monlang-LV2/expr/FunctionCall.h>
#include <monlang-LV2/expr/Lambda.h>
#include <monlang-LV2/expr/BlockExpression.h>
#include <monlang-LV2/expr/SpecialSymbol.h>
#include <monlang-LV2/expr/Literal.h>
#include <monlang-LV2/expr/Lvalue.h>

#include <monlang-LV1/ast/ParenthesesGroup.h>
#include <monlang-LV1/ast/Atom.h>
/* only required to set_token_len on a Word */
#include <monlang-LV1/ast/CurlyBracketsGroup.h>
#include <monlang-LV1/ast/PostfixParenthesesGroup.h>
#include <monlang-LV1/ast/PostfixSquareBracketsGroup.h>
#include <monlang-LV1/ast/Association.h>

#include <utils/assert-utils.h>
#include <utils/mem-utils.h>
#include <utils/vec-utils.h>
#include <utils/variant-utils.h>
#include <utils/defer-util.h>

#define unless(x) if (!(x))

static void fixOperandTokenLen(MayFail<MayFail_<Operation>>&, std::stack<Alteration>&, size_t& alt_count);

MayFail<Expression_> buildExpression(const Term& term) {
    ASSERT (term.words.size() > 0);
    auto term_ = term; // local non-const working variable
    // std::stack<Alteration> alterations; // required to adjust Operation operand _tokenLen
    static thread_local std::stack<Alteration> alterations; // required to adjust Operation operand _tokenLen
                                                            // ..accoding to fixPrecedence()
    static thread_local size_t alt_count = 0; // count of handled LEFT/RIGHT_OPND alterations
    static thread_local size_t recursive_call = 0;
    if (!recursive_call++) {
        /* if parent call.. */
        alterations = std::stack<Alteration>(); // reset alterations
        alt_count = 0; // reset alt_count
    }
    defer {recursive_call--;};

    BEGIN:

    unless (term_.words.size() % 2 == 1) {
        return Malformed(Expression_(), ERR(161));
    }

    /* odd-indexed words, starting at [1], must be Atoms..
        ..which value matches an operator from the precedence table */
    for (size_t i = 1; i < term_.words.size(); i += 2) {
        auto optr_found = false;

        for (auto [operators, _]: PRECEDENCE_TABLE) {
            unless (std::holds_alternative<Atom*>(term_.words[i])) {
                return Malformed(Expression_(), ERR(162));
            }
            auto optor = std::get<Atom*>(term_.words[i])->value;

            if (vec_contains(operators, optor)) {
                optr_found = true;
                break;
            }
        }

        unless (optr_found) {
            return Malformed(Expression_(), ERR(163));
        }
    }

    // ASSERT (term_ =~ "Word (OPERATOR Word)*"_);

    auto unalteredTerm = term_; // required to set Operation _tokenLen
    fixPrecedence(term_, /*OUT*/alterations);
    ASSERT (term_.words.size() == 1 || term_.words.size() == 3);

    // if (term_ =~ "Word Word Word"_) {
    //     ..
    // }

    if (peekOperation(term_)) {
        auto operation = buildOperation(term_);
        operation.val._tokenLen = unalteredTerm._tokenLen;
        if (!alterations.empty()) {
            fixOperandTokenLen(operation, alterations, alt_count); // uncount implicit parentheses (syntax transformation by fixPrecedence(), then counted after unwrapping expr)
        }

        /*
            it's ok if operation _tokenLen looks wrong at this point,
            if it's going to be used as an operand,
            then it will be fixed by fixOperandTokenLen() at
            the time of building the parent operation
        */

        return mayfail_convert<Expression_>(operation);
    }

    // 'Operation' was the only Expression with multiple words
    ASSERT (term_.words.size() == 1);
    Word word = term_.words[0];
    // keep term token length
    set_token_len(word, term_._tokenLen);

    // if (word =~ "PostfixParenthesesGroup"_) {
    //     return mayfail_convert<Expression_>(buildFunctionCall(word));
    // }

    if (peekFunctionCall(word)) {
        return mayfail_convert<Expression_>(buildFunctionCall(word));
    }

    // if (word =~ "Association<"
    //                  "ParenthesesGroup<Term<Atom>*>,"
    //                  "CurlyBracketsGroup"
    //              ">"_) {
    //     return mayfail_convert<Expression_>(buildLambda(word));
    // }

    if (peekLambda(word)) {
        return mayfail_convert<Expression_>(buildLambda(word));
    }

    // if (word =~ "CurlyBracketsGroup"_) {
    //     return mayfail_convert<Expression_>(buildBlockExpression(word));
    // }

    if (peekBlockExpression(word)) {
        return mayfail_convert<Expression_>(buildBlockExpression(word));
    }

    // if (word =~ "Atom<$.*>"_) {
    //     return mayfail_convert<Expression_>(buildSpecialSymbol(word));
    // }

    if (peekSpecialSymbol(word)) {
        return (Expression_)move_to_heap(buildSpecialSymbol(word));
    }

    // if (word =~ "Atom<[0-9]+>"_) {
    //     return mayfail_convert<Expression_>(buildLiteral(word));
    // }

    if (peekLiteral(word)) {
        return (Expression_)move_to_heap(buildLiteral(word));
    }

    // if (word =~ "Atom"_) {
    //     return mayfail_convert<Expression_>(buildLvalue(word));
    // }

    if (peekLvalue(word)) {
        return (Expression_)move_to_heap(buildLvalue(word));
    }

    // // if grouped expression => unwrap then go back to beginning
    // if (word =~ "ParenthesesGroup<Term<Word+>>"_) {
    //     auto group = *std::get<ParenthesesGroup*>(word);
    //     term_ = group.terms.at(0);
    //     goto BEGIN; // prevent unnecessary recursive call
    // }

    if (std::holds_alternative<ParenthesesGroup*>(word)) {
        auto group = *std::get<ParenthesesGroup*>(word);
        // if grouped expression => unwrap then go back to beginning
        if (group.terms.size() == 1) {
            auto saveTokenLen = term_._tokenLen;
            term_ = group.terms[0];
            term_._tokenLen = saveTokenLen;

            // // if we unwrap explicit parentheses => reset alt_count back to 0
            // if (alterations.empty() || alterations.top() == Alteration::NONE) {
            //     alt_count = 0;
            // }

            goto BEGIN; // prevent unnecessary recursive call
        }
    }

    /* reached fall-through */
    return Malformed(Expression_(), ERR(169));
}

static void fixOperandTokenLen(MayFail<MayFail_<Operation>>& operation, std::stack<Alteration>& alterations, size_t& alt_count) {
    ASSERT (!alterations.empty());

    if (alterations.top() == Alteration::LEFT_OPND) {
        size_t nb_of_chars_to_remove = ++alt_count * (
            sequenceLen(ParenthesesGroup::INITIATOR_SEQUENCE)
            + sequenceLen(ParenthesesGroup::TERMINATOR_SEQUENCE)
        );
        size_t newTokenLen = token_len(operation.val.leftOperand.val)
                - nb_of_chars_to_remove;
        set_token_len(operation.val.leftOperand.val, newTokenLen);
    }

    else if (alterations.top() == Alteration::RIGHT_OPND) {
        size_t nb_of_chars_to_remove = ++alt_count * (
            sequenceLen(ParenthesesGroup::INITIATOR_SEQUENCE)
            + sequenceLen(ParenthesesGroup::TERMINATOR_SEQUENCE)
        );
        size_t newTokenLen = token_len(operation.val.rightOperand.val)
                - nb_of_chars_to_remove;
        set_token_len(operation.val.rightOperand.val, newTokenLen);
    }

    else /* ::NONE (or ::SKIP ?) */ {
        ; // do nothing
        alt_count = 0; // doesn't work for test-9772
    }

    alterations.pop();
}

Expression unwrap_expr(Expression_ expression) {
    return std::visit(overload{
        [](Literal* expr) -> Expression {return expr;},
        [](SpecialSymbol* expr) -> Expression {return expr;},
        [](Lvalue* expr) -> Expression {return expr;},
        [](auto* mf_) -> Expression {return move_to_heap(unwrap(*mf_));},
    }, expression);
}

Expression_ wrap_expr(Expression expression) {
    return std::visit(overload{
        [](Literal* expr) -> Expression_ {return expr;},
        [](SpecialSymbol* expr) -> Expression_ {return expr;},
        [](Lvalue* expr) -> Expression_ {return expr;},
        [](auto* mf_) -> Expression_ {return move_to_heap(wrap(*mf_));},
    }, expression);
}
