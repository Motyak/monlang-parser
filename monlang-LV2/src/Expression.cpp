#include <monlang-LV2/Expression.h>

/* impl only */
#include <monlang-LV2/precedence.h> // fixPrecedence()
#include <monlang-LV2/expr/Operation.h>
#include <monlang-LV2/expr/FunctionCall.h>
#include <monlang-LV2/expr/Lambda.h>
#include <monlang-LV2/expr/BlockExpression.h>
#include <monlang-LV2/expr/Literal.h>
#include <monlang-LV2/expr/Lvalue.h>

#include <monlang-LV1/ast/ParenthesesGroup.h>
#include <monlang-LV1/ast/Atom.h>

#include <utils/assert-utils.h>
#include <utils/mem-utils.h>
#include <utils/vec-utils.h>

#define unless(x) if (!(x))

#define FALLTHROUGH() \
    fallthrough = true; \
    return Expression()

Expression buildExpression(const Term& term, const context_t& cx) {
    auto& fallthrough = cx.fallthrough;
    ASSERT (!fallthrough);
    ASSERT (term.words.size() > 0);
    auto term_ = term; // local non-const working variable

    BEGIN:

    // unless (term_ =~ "Word (OPERATOR Word)*"_) {
    //     FALLTHROUGH();
    // }
    // // covers both blocks below

    unless (term_.words.size() % 2 == 1) {
        FALLTHROUGH();
    }

    /* odd-indexed words, starting at [1], must be Atoms..
        ..which value matches an operator from the precedence table */
    for (size_t i = 1; i < term_.words.size(); i += 2) {
        auto optr_found = false;

        for (auto [operators, _]: PRECEDENCE_TABLE) {
            unless (std::holds_alternative<Atom*>(term_.words[i])) {
                FALLTHROUGH();
            }
            auto optor = std::get<Atom*>(term_.words[i])->value;

            if (vec_contains(operators, optor)) {
                optr_found = true;
                break;
            }
        }

        unless (optr_found) {
            FALLTHROUGH();
        }
    }

    fixPrecedence(term_);
    ASSERT (term_.words.size() == 1 || term_.words.size() == 3);

    // if (term_ =~ "Word Word Word"_) {
    //     return move_to_heap(buildOperation(term, cx));
    // }

    if (peekOperation(term)) {
        return move_to_heap(buildOperation(term, cx));
    }

    // 'Operation' was the only Expression with multiple words
    ASSERT (term_.words.size() == 1);
    Word word = term_.words[0];

    // if (word =~ "PostfixParenthesesGroup"_) {
    //     return move_to_heap(buildFunctionCall(word, cx));
    // }

    if (peekFunctionCall(word)) {
        return move_to_heap(buildFunctionCall(word, cx));
    }

    // if (word =~ "Association<"
    //                  "ParenthesesGroup<Term<Atom>*>,"
    //                  "CurlyBracketsGroup"
    //              ">"_) {
    //     return move_to_heap(buildLambda(word, cx));
    // }

    if (peekLambda(word)) {
        return move_to_heap(buildLambda(word, cx));
    }

    // if (word =~ "CurlyBracketsGroup"_) {
    //     return move_to_heap(buildBlockExpression(word, cx));
    // }

    if (peekBlockExpression(word)) {
        return move_to_heap(buildBlockExpression(word, cx));
    }

    // if (word =~ "Atom<[0-9]+>"_) {
    //     return move_to_heap(buildLiteral(word));
    // }

    if (peekLiteral(word)) {
        return move_to_heap(buildLiteral(word));
    }

    // if (word =~ "Atom"_) {
    //     return move_to_heap(buildLiteral(word));
    // }

    if (peekLvalue(word)) {
        return move_to_heap(buildLvalue(word));
    }

    // // if grouped expression => unwrap then go back to beginning
    // if (word =~ "ParenthesesGroup<Word>"_) {
    //     auto group = *std::get<ParenthesesGroup*>(word);
    //     term_ = group.terms.at(0);
    //     goto BEGIN; // prevent unnecessary recursive call
    // }

    if (std::holds_alternative<ParenthesesGroup*>(word)) {
        auto group = *std::get<ParenthesesGroup*>(word);
        // if grouped expression => unwrap then go back to beginning
        if (group.terms.size() == 1) {
            term_ = group.terms[0];
            goto BEGIN; // prevent unnecessary recursive call
        }
    }

    /* reached fall-through */
    fallthrough = true;
    return Expression(); // return stub
}
