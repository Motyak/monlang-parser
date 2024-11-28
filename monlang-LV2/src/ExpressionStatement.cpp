#include <monlang-LV2/ExpressionStatement.h>

#include <utils/assert-utils.h>
#include <utils/variant-utils.h>

static Word pw2w(const ProgramWord& pw) {
    return std::visit(overload{
        [](SquareBracketsTerm*) -> Word {SHOULD_NOT_HAPPEN();},
        [](auto word) -> Word {return word;},
    }, pw);
}

static Term toTerm(const ProgramSentence& sentence) {
    ASSERT (sentence.programWords.size() > 0);
    std::vector<Word> words;
    for (auto e: sentence.programWords) {
        words.push_back(pw2w(e));
    }
    return Term{words};
}

ExpressionStatement buildExpressionStatement(const ProgramSentence& sentence, const context_t& cx) {
    auto& term = cx.term;
    ASSERT (sentence.programWords.size() > 0);

    term = toTerm(sentence);
    return ExpressionStatement{buildExpression(term, cx)};
}
