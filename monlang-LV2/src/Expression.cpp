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

#include <utils/assert-utils.h>
#include <utils/mem-utils.h>
#include <utils/vec-utils.h>
#include <utils/variant-utils.h>

#define unless(x) if (!(x))

MayFail<Expression_> buildExpression(const Term& term) {
    ASSERT (term.words.size() > 0);
    auto term_ = term; // local non-const working variable
    size_t groupNesting = 0;

    BEGIN:

    unless (term_.words.size() % 2 == 1) {
        auto expr = StubExpression_();
        set_group_nesting(expr, groupNesting);
        return Malformed<Expression_>(expr, ERR(161));
    }

    /* odd-indexed words, starting at [1], must be Atoms..
        ..which value matches an operator from the precedence table */
    for (size_t i = 1; i < term_.words.size(); i += 2) {
        auto optr_found = false;

        for (auto [operators, _]: PRECEDENCE_TABLE) {
            unless (std::holds_alternative<Atom*>(term_.words[i])) {
                auto expr = StubExpression_();
                set_group_nesting(expr, groupNesting);
                return Malformed<Expression_>(expr, ERR(162));
            }
            auto optor = std::get<Atom*>(term_.words[i])->value;

            if (vec_contains(operators, optor)) {
                optr_found = true;
                break;
            }
        }

        unless (optr_found) {
            auto expr = StubExpression_();
            set_group_nesting(expr, groupNesting);
            return Malformed<Expression_>(expr, ERR(163));
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
        operation.val._groupNesting = groupNesting;
        return mayfail_convert<Expression_>(operation);
    }

    // 'Operation' was the only Expression with multiple words
    ASSERT (term_.words.size() == 1);
    auto word = (Word)term_;

    // if (word =~ "PostfixParenthesesGroup"_) {
    //     return mayfail_convert<Expression_>(buildFunctionCall(word));
    // }

    if (peekFunctionCall(word)) {
        auto functionCall = buildFunctionCall(word);
        functionCall.val._groupNesting = groupNesting;
        return mayfail_convert<Expression_>(functionCall);
    }

    // if (word =~ "Association<"
    //                  "ParenthesesGroup<Term<Atom>*>,"
    //                  "CurlyBracketsGroup"
    //              ">"_) {
    //     return mayfail_convert<Expression_>(buildLambda(word));
    // }

    if (peekLambda(word)) {
        auto lambda = buildLambda(word);
        lambda.val._groupNesting = groupNesting;
        return mayfail_convert<Expression_>(lambda);
    }

    // if (word =~ "CurlyBracketsGroup"_) {
    //     return mayfail_convert<Expression_>(buildBlockExpression(word));
    // }

    if (peekBlockExpression(word)) {
        auto blockExpr = buildBlockExpression(word);
        blockExpr.val._groupNesting = groupNesting;
        return mayfail_convert<Expression_>(blockExpr);
    }

    // if (word =~ "Atom<$.*>"_) {
    //     return mayfail_convert<Expression_>(buildSpecialSymbol(word));
    // }

    if (peekSpecialSymbol(word)) {
        auto specialSymbol = buildSpecialSymbol(word);
        specialSymbol._groupNesting = groupNesting;
        return (Expression_)move_to_heap(specialSymbol);
    }

    // if (word =~ "Atom<[0-9]+>"_) {
    //     return mayfail_convert<Expression_>(buildLiteral(word));
    // }

    if (peekLiteral(word)) {
        auto literal = buildLiteral(word);
        literal._groupNesting = groupNesting;
        return (Expression_)move_to_heap(literal);
    }

    // if (word =~ "Atom"_) {
    //     return mayfail_convert<Expression_>(buildLvalue(word));
    // }

    if (peekLvalue(word)) {
        auto lvalue = buildLvalue(word);
        lvalue._groupNesting = groupNesting;
        return (Expression_)move_to_heap(lvalue);
    }

    // // if grouped expression => unwrap then go back to beginning
    // if (word =~ "ParenthesesGroup<Term<Word+>>"_) {
    //     auto group = *std::get<ParenthesesGroup*>(word);
    //     term_ = group.terms.at(0);
    //     goto BEGIN; // prevent unnecessary recursive call
    // }

    // TODO: unwrap as long as there are nested groups, before doing the 'goto BEGIN',
    // .. will also prevent using the local variable `nestingGroup` at the very top
    if (std::holds_alternative<ParenthesesGroup*>(word)) {
        auto group = *std::get<ParenthesesGroup*>(word);
        // if grouped expression => unwrap then go back to beginning
        if (group.terms.size() == 1) {
            groupNesting += 1;
            auto saveTokenLen = term_._tokenLen;
            term_ = group.terms[0];
            term_._tokenLen = saveTokenLen;

            goto BEGIN; // prevent unnecessary recursive call
        }
    }

    /* reached fall-through */
    auto expr = StubExpression_();
    set_group_nesting(expr, groupNesting);
    return Malformed<Expression_>(expr, ERR(169));
}

Expression_ StubExpression_() {
    return move_to_heap(_StubExpression_{});
}

Expression unwrap_expr(Expression_ expression) {
    return std::visit(overload{
        [](Literal* expr) -> Expression {return expr;},
        [](SpecialSymbol* expr) -> Expression {return expr;},
        [](Lvalue* expr) -> Expression {return expr;},
        [](_StubExpression_*) -> Expression {SHOULD_NOT_HAPPEN();},
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
