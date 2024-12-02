#include <monlang-LV2/stmt/ForeachStatement.h>

/* impl only */
#include <monlang-LV1/ast/Atom.h>

#include <utils/assert-utils.h>

#define unless(x) if(!(x))

bool peekForeachStatement(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 3) {
        return false;
    }
    unless (std::holds_alternative<Atom*>(sentence.programWords[0])) {
        return false;
    }
    auto atom = *std::get<Atom*>(sentence.programWords[0]);
    return atom.value == "foreach";
}

// returns empty opt in case any non-word
static std::optional<Term> extractIterable(const ProgramSentence&);

ForeachStatement buildForeachStatement(const ProgramSentence& sentence, const context_t& cx) {
    ASSERT (!cx.malformed_stmt && !cx.fallthrough);
    ASSERT (sentence.programWords.size() >= 3);

    auto iterable_as_term = extractIterable(sentence);
    unless (iterable_as_term) {
        cx.malformed_stmt = "iterable isn't a valid expression";
        return ForeachStatement(); // stub
    }
    auto iterable = buildExpression(*iterable_as_term, cx);
    if (cx.fallthrough) {
        cx.malformed_stmt = "iterable isn't a valid expression";
        return ForeachStatement(); // stub
    }

    auto pw = sentence.programWords.back();
    unless (holds_word(pw)) {
        cx.malformed_stmt = "invalid foreach block";
        return ForeachStatement(); // stub
    }
    auto word = get_word(pw);
    auto block = buildBlockExpression(word, cx);
    if (cx.fallthrough) {
        cx.malformed_stmt = "invalid foreach block";
        return ForeachStatement(); // stub
    }

    return ForeachStatement{iterable, block};
}

static std::optional<Term> extractIterable(const ProgramSentence& sentence) {
    auto rhs_as_sentence = std::vector<ProgramWord>(
        sentence.programWords.begin() + 1,
        sentence.programWords.end() - 1
    );

    std::vector<Word> words;
    for (auto e: rhs_as_sentence) {
        unless (holds_word(e)) {
            return {};
        }
        // perform a deep copy of the words instead
        words.push_back(get_word(e));
    }
    return Term{words};
}
