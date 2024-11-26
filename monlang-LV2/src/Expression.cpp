#include <monlang-LV2/Expression.h>

/* impl only */
#include <monlang-LV2/FunctionCall.h>
#include <monlang-LV2/Lambda.h>
#include <monlang-LV2/BlockExpression.h>
#include <monlang-LV2/Literal.h>
#include <monlang-LV2/Lvalue.h>

#include <monlang-LV1/ast/ParenthesesGroup.h>

#include <utils/assert-utils.h>
#include <utils/mem-utils.h>

Expression buildExpression(const Term& term, const context_t& cx) {
    auto& fallthrough = cx.fallthrough;
    ASSERT (term.words.size() > 0);
    auto term_ = term; // local non-const working variable
    Word word;

    BEGIN:

    // if (term =~ "EXPRESSION (BINARY-OPERATOR EXPRESSION)*"_) {
    //     return buildOperation(term);
    // }

    // since 'Operation' is the only Expression with multiple words..
    // ..we already know none will possibly match
    if (term_.words.size() > 1) {
        goto FALLTHROUGH;
    }
    word = term_.words[0];

    // ...

    if (peekFunctionCall(word)) {
        return move_to_heap(buildFunctionCall(word, cx));
    }

    if (peekLambda(word)) {
        return move_to_heap(buildLambda(word, cx));
    }

    if (peekBlockExpression(word)) {
        return move_to_heap(buildBlockExpression(word, cx));
    }

    if (peekLiteral(word)) {
        return move_to_heap(buildLiteral(word));
    }

    if (peekLvalue(word)) {
        return move_to_heap(buildLvalue(word));
    }

    // if grouped expression => unwrap then go back to beginning
    if (std::holds_alternative<ParenthesesGroup*>(word)) {
        auto group = *std::get<ParenthesesGroup*>(word);
        if (group.terms.size() == 1) {
            term_ = group.terms[0];
            goto BEGIN; // prevent unnecessary recursive call
        }
    }

    FALLTHROUGH:
    /* reached fall-through */
    fallthrough = true;
    return Expression(); // return stub
}
