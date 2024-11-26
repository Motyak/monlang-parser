#include <monlang-LV2/ExpressionStatement.h>
#include <monlang-LV2/common.h>

#include <utils/assert-utils.h>

ExpressionStatement buildExpressionStatement(const ProgramSentence& sentence, const context_t& cx) {
    auto& term = cx.term;
    ASSERT (sentence.programWords.size() > 0);

    term = toTerm(sentence);
    return ExpressionStatement{buildExpression(term, cx)};
}
