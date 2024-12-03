#include <monlang-LV2/stmt/ReturnStatement.h>

/* impl only */

#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/ProgramSentence.h>

#include <utils/assert-utils.h>

#define unless(x) if(!(x))

#define MALFORMED_STMT(err_msg) \
    malformed_stmt = err_msg; \
    return ReturnStatement()

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

// where 'value' are the [1], [2], .. words from the sentence
// ..returns empty opt if any non-word
static std::optional<Term> extractValue(const ProgramSentence&);

ReturnStatement buildReturnStatement(const ProgramSentence& sentence, context_t* cx) {
    auto& malformed_stmt = *cx->malformed_stmt;
    auto& fallthrough = *cx->fallthrough;

    ASSERT (!malformed_stmt && !fallthrough);
    auto value = std::optional<Expression>();
    if (sentence.programWords.size() >= 1) {
        auto value_as_term = extractValue(sentence);
        unless (value_as_term) {
            MALFORMED_STMT("value is an unknown Expression");
        }
        value = buildExpression(*value_as_term, cx);
        if (fallthrough) {
            MALFORMED_STMT("value is an unknown Expression");
        }
    }
    return ReturnStatement{value};
}

static std::optional<Term> extractValue(const ProgramSentence& sentence) {
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
