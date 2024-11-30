#include <monlang-LV2/stmt/LetStatement.h>

/* impl only */

#include <monlang-LV1/ast/Atom.h>

#include <utils/assert-utils.h>

#define unless(x) if(!(x))

bool peekLetStatement(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 3) {
        return false;
    }

    auto pw = sentence.programWords[0];
    unless (std::holds_alternative<Atom*>(pw)) {
        return false;
    }

    auto atom = *std::get<Atom*>(pw);
    return atom.value == "let";
}

// where rhs are the [2], [3], .. words from the sentence
static Term extractRhs(const ProgramSentence&);

LetStatement buildLetStatement(const ProgramSentence& sentence, const context_t& cx) {
    ASSERT (!cx.fallthrough);
    ASSERT (sentence.programWords.size() >= 3);

    unless (holds_word(sentence.programWords[1])) {
        cx.malformed_stmt = "lhs is not an identifier";
        return LetStatement(); // stub
    }
    auto word = get_word(sentence.programWords[1]);
    ASSERT (std::holds_alternative<Atom*>(word));
    auto atom = *std::get<Atom*>(word);
    auto lhs = atom.value;

    auto rhs_as_term = extractRhs(sentence);
    auto rhs = buildExpression(rhs_as_term, cx);
    if (cx.fallthrough) {
        cx.malformed_stmt = "rhs is an unknown Expression";
        return LetStatement(); // stub
    }

    return LetStatement{lhs, rhs};
}

static Term extractRhs(const ProgramSentence& sentence) {
    auto rhs_as_sentence = std::vector<ProgramWord>(
        sentence.programWords.begin() + 2,
        sentence.programWords.end()
    );

    std::vector<Word> words;
    for (auto e: rhs_as_sentence) {
        words.push_back(get_word(e));
    }
    return Term{words};
}
