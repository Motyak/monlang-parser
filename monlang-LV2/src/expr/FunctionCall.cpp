#include <monlang-LV2/expr/FunctionCall.h>
#include <monlang-LV2/Expression.h>

#include <monlang-LV1/PostfixParenthesesGroup.h>

#include <utils/assert-utils.h>

#define unless(x) if(!(x))

bool peekFunctionCall(const Word& word) {
    return std::holds_alternative<PostfixParenthesesGroup*>(word);
}

FunctionCall buildFunctionCall(const Word& word, context_t* cx) {
    auto& fallthrough = *cx->fallthrough;

    ASSERT (!fallthrough);
    ASSERT (std::holds_alternative<PostfixParenthesesGroup*>(word));
    auto ppg = *std::get<PostfixParenthesesGroup*>(word);
    auto function = buildExpression(Term{{ppg.leftPart}}, cx);
    if (fallthrough) {
        return FunctionCall(); // stub
    }

    std::vector<Expression> arguments;
    for (auto term: ppg.rightPart.terms) {
        auto expression = buildExpression(term, cx);
        if (fallthrough) {
            return FunctionCall(); // stub
        }
        arguments.push_back(expression);
    }

    return FunctionCall{function, arguments};
}
