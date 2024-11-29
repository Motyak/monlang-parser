#include <monlang-LV2/stmt/ExpressionStatement.h>

#include <utils/assert-utils.h>
#include <utils/variant-utils.h>

static Term toTerm(const ProgramSentence& sentence) {
    std::vector<Word> words;
    for (auto e: sentence.programWords) {
        words.push_back(get_word(e));
    }
    return Term{words};
}

ExpressionStatement buildExpressionStatement(const ProgramSentence& sentence, const context_t& cx) {
    auto& term = cx.term;
    ASSERT (sentence.programWords.size() > 0);

    term = toTerm(sentence);
    return ExpressionStatement{buildExpression(term, cx)};
}
