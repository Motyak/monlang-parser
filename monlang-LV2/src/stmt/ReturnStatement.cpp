#include <monlang-LV2/stmt/ReturnStatement.h>

/* impl only */

#include <monlang-LV1/ast/Atom.h>

#include <utils/assert-utils.h>

#define unless(x) if(!(x))

bool peekReturnStatement(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 1) {
        return false;
    }

    auto pw = sentence.programWords[0];
    unless (std::holds_alternative<Atom*>(pw)) {
        return false;
    }

    auto atom = *std::get<Atom*>(pw);
    return atom.value == "return";
}

// where rhs are the [1], [2], .. words from the sentence
// ..returns empty opt if any non-word
static std::optional<Term> extractRhs(const ProgramSentence&);

ReturnStatement buildReturnStatement(const ProgramSentence& sentence, const context_t& cx) {
    ASSERT (!cx.malformed_stmt && !cx.fallthrough);
    auto expr = std::optional<Expression>();
    if (sentence.programWords.size() >= 1) {
        auto rhs_as_term = extractRhs(sentence);
        unless (rhs_as_term) {
            cx.malformed_stmt = "returned Expression is unknown";
            return ReturnStatement(); // stub
        }
        expr = buildExpression(*rhs_as_term, cx);
        if (cx.fallthrough) {
            cx.malformed_stmt = "returned Expression is unknown";
            return ReturnStatement(); // stub
        }
    }
    return ReturnStatement{expr};
}

static std::optional<Term> extractRhs(const ProgramSentence& sentence) {
    auto rhs_as_sentence = std::vector<ProgramWord>(
        sentence.programWords.begin() + 1,
        sentence.programWords.end()
    );

    std::vector<Word> words;
    for (auto e: rhs_as_sentence) {
        unless (holds_word(e)) {
            return {};
        }
        words.push_back(get_word(e));
    }
    return Term{words};
}
