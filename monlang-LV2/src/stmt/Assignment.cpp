#include <monlang-LV2/stmt/Assignment.h>

#include <monlang-LV1/ast/Atom.h>

#include <utils/assert-utils.h>

#define unless(x) if(!(x))

bool peekAssignment(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 3) {
        return false;
    }

    auto pw = sentence.programWords[1];
    unless (std::holds_alternative<Atom*>(pw)) {
        return false;
    }

    auto atom = *std::get<Atom*>(pw);
    return atom.value == ":=";
}

// where rhs are the [2], [3], .. words from the sentence
// ..returns empty opt if any non-word
static std::optional<Term> extractRhs(const ProgramSentence&);

Assignment buildAssignment(const ProgramSentence& sentence, const context_t& cx) {
    ASSERT (!cx.fallthrough);
    ASSERT (sentence.programWords.size() >= 3);

    unless (holds_word(sentence.programWords[0])) {
        cx.malformed_stmt = "lhs is not a Lvalue";
        return Assignment(); // stub
    }
    auto word = get_word(sentence.programWords[0]);
    unless (peekLvalue(word)) {
        cx.malformed_stmt = "lhs is not an Lvalue";
        return Assignment(); // stub
    }
    auto lhs = buildLvalue(word);

    auto rhs_as_term = extractRhs(sentence);
    unless (rhs_as_term) {
        cx.malformed_stmt = "rhs is an unknown Expression";
        return Assignment(); // stub
    }
    auto rhs = buildExpression(*rhs_as_term, cx);
    if (cx.fallthrough) {
        cx.malformed_stmt = "rhs is an unknown Expression";
        return Assignment(); // stub
    }

    return Assignment{lhs, rhs};
}

static std::optional<Term> extractRhs(const ProgramSentence& sentence) {
    auto rhs_as_sentence = std::vector<ProgramWord>(
        sentence.programWords.begin() + 2,
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
