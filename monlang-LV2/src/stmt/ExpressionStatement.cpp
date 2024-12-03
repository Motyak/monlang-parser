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

ExpressionStatement buildExpressionStatement(const ProgramSentence& sentence, context_t* cx) {
    auto& malformed_stmt = *cx->malformed_stmt;
    auto& fallthrough = *cx->fallthrough;
    auto& term = *cx->term;

    ASSERT (!malformed_stmt && !fallthrough);
    ASSERT (sentence.programWords.size() > 0);

    term = toTerm(sentence);
    return ExpressionStatement{buildExpression(term, cx)};
}
