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

MayFail<Expression_> buildExpression(const Term& term) {
    ASSERT (term.words.size() > 0);
    auto term_ = term; // local non-const working variable

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

    fixPrecedence(term_);
    ASSERT (term_.words.size() == 1 || term_.words.size() == 3);

    // if (term_ =~ "Word Word Word"_) {
    //     ..
    // }

    if (peekOperation(term_)) {
        auto operation = buildOperation(term_);
        operation.val._tokenLen = term_._tokenLen;

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

            goto BEGIN; // prevent unnecessary recursive call
        }
    }

    /* reached fall-through */
    return Malformed(Expression_(), ERR(169));
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
