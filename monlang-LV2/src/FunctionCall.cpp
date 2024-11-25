#include <monlang-LV2/FunctionCall.h>
#include <monlang-LV2/Rvalue.h>

#include <monlang-LV1/PostfixParenthesesGroup.h>

#include <utils/assert-utils.h>

#define unless(x) if(!(x))

bool peekFunctionCall(const Word& word) {
    return std::holds_alternative<PostfixParenthesesGroup*>(word);
}

FunctionCall buildFunctionCall(const Word& word, const context_t& cx) {
    ASSERT (!cx.fallthrough);
    ASSERT (std::holds_alternative<PostfixParenthesesGroup*>(word));
    auto ppg = *std::get<PostfixParenthesesGroup*>(word);
    auto function = buildRvalue(Term{{ppg.leftPart}}, cx);

    std::vector<Rvalue> arguments;
    for (auto term: ppg.rightPart.terms) {
        auto rvalue = buildRvalue(term, cx);
        if (cx.fallthrough) {
            return FunctionCall(); // stub
        }
        arguments.push_back(rvalue);
    }

    return FunctionCall{function, arguments};
}
