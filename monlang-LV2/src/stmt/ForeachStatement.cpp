#include <monlang-LV2/stmt/ForeachStatement.h>

/* impl only */
#include <monlang-LV1/ast/Atom.h>

#include <utils/assert-utils.h>

#define unless(x) if(!(x))

#define MALFORMED_STMT(err_msg) \
    malformed_stmt = err_msg; \
    return ForeachStatement()

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

ForeachStatement buildForeachStatement(const ProgramSentence& sentence, context_t* cx) {
    auto& malformed_stmt = *cx->malformed_stmt;
    auto& fallthrough = *cx->fallthrough;

    ASSERT (!malformed_stmt && !fallthrough);
    ASSERT (sentence.programWords.size() >= 3);

    auto iterable_as_term = extractIterable(sentence);
    unless (iterable_as_term) {
        MALFORMED_STMT("iterable isn't a valid expression");
    }
    auto iterable = buildExpression(*iterable_as_term, cx);
    if (fallthrough) {
        MALFORMED_STMT("iterable isn't a valid expression");
    }

    auto pw = sentence.programWords.back();
    unless (holds_word(pw)) {
        MALFORMED_STMT("invalid foreach block");
    }
    auto word = get_word(pw);
    auto block = buildBlockExpression(word, cx);
    if (fallthrough) {
        MALFORMED_STMT("invalid foreach block");
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
