#include <monlang-LV2/expr/FunctionCall.h>

/* impl only */
#include <monlang-LV1/ast/PostfixParenthesesGroup.h>

#include <utils/assert-utils.h>

#define unless(x) if(!(x))

bool peekFunctionCall(const Word& word) {
    return std::holds_alternative<PostfixParenthesesGroup*>(word);
}

MayFail<MayFail_<FunctionCall>> buildFunctionCall(const Word& word) {
    ASSERT (std::holds_alternative<PostfixParenthesesGroup*>(word));
    auto ppg = *std::get<PostfixParenthesesGroup*>(word);
    auto function = buildExpression((Term)ppg.leftPart);
    if (function.has_error()) {
        return Malformed(MayFail_<FunctionCall>{}, ERR(621));
    }

    std::vector<MayFail<Expression_>> arguments;
    for (auto term: ppg.rightPart.terms) {
        auto expression = buildExpression(term);
        arguments.push_back(expression);
        if (expression.has_error()) {
            return Malformed(MayFail_<FunctionCall>{function, arguments}, ERR(622));
        }
    }

    auto functionCall = MayFail_<FunctionCall>{function, arguments};
    functionCall._tokenLen = ppg._tokenLen;
    return functionCall;
}

FunctionCall::FunctionCall(const Expression& function, const std::vector<Expression>& arguments)
        : function(function), arguments(arguments){}

MayFail_<FunctionCall>::MayFail_(MayFail<Expression_> function, std::vector<MayFail<Expression_>> arguments)
        : function(function), arguments(arguments){}

MayFail_<FunctionCall>::MayFail_(FunctionCall functionCall) {
    auto function = wrap_expr(functionCall.function);
    auto arguments = std::vector<MayFail<Expression_>>();
    for (auto e: functionCall.arguments) {
        arguments.push_back(wrap_expr(e));
    }

    this->function = function;
    this->arguments = arguments;
    this->_tokenLen = functionCall._tokenLen;
}

MayFail_<FunctionCall>::operator FunctionCall() const {
    auto function = unwrap_expr(this->function.value());
    auto arguments = std::vector<Expression>();
    for (auto e: this->arguments) {
        arguments.push_back(unwrap_expr(e.value()));
    }
    auto functionCall = FunctionCall{function, arguments};
    functionCall._tokenLen = this->_tokenLen;
    return functionCall;
}
