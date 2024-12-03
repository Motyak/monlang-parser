#include <monlang-LV2/stmt/Accumulation.h>
#include <monlang-LV2/precedence.h>

/* impl only */

#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/ProgramSentence.h>

#include <utils/vec-utils.h>
#include <utils/assert-utils.h>

#define unless(x) if(!(x))

#define MALFORMED_STMT(err_msg) \
    malformed_stmt = err_msg; \
    return Accumulation()

bool peekAccumulation(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 3) {
        return false;
    }

    auto pw = sentence.programWords[1];
    unless (std::holds_alternative<Atom*>(pw)) {
        return false;
    }

    auto atom = *std::get<Atom*>(pw);
    unless (atom.value.back() == '=') {
        return false;
    }
    auto optr = atom.value.substr(0, atom.value.size() - 1);

    auto optr_found = false;
    for (auto [operators, _]: PRECEDENCE_TABLE) {
        if (vec_contains(operators, optr)) {
            optr_found = true;
            break;
        }
    }

    return optr_found;
}

// where rhs are the [2], [3], .. words from the sentence
// ..returns empty opt if any non-word
static std::optional<Term> extractRhs(const ProgramSentence&);

Accumulation buildAccumulation(const ProgramSentence& sentence, context_t* cx) {
    auto& malformed_stmt = *cx->malformed_stmt;
    auto& fallthrough = *cx->fallthrough;

    ASSERT (!malformed_stmt && !fallthrough);
    ASSERT (sentence.programWords.size() >= 3);

    unless (holds_word(sentence.programWords[0])) {
        MALFORMED_STMT("lhs is not a Lvalue");
    }
    auto word = get_word(sentence.programWords[0]);
    unless (peekLvalue(word)) {
        MALFORMED_STMT("lhs is not an Lvalue");
    }
    auto lhs = buildLvalue(word);

    ASSERT (std::holds_alternative<Atom*>(sentence.programWords[1]));
    auto atom = *std::get<Atom*>(sentence.programWords[1]);
    auto optr = atom.value.substr(0, atom.value.size() - 1);

    auto rhs_as_term = extractRhs(sentence);
    unless (rhs_as_term) {
        MALFORMED_STMT("rhs is an unknown Expression");
    }
    auto rhs = buildExpression(*rhs_as_term, cx);
    if (fallthrough) {
        MALFORMED_STMT("rhs is an unknown Expression");
    }

    return Accumulation{lhs, optr, rhs};
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
