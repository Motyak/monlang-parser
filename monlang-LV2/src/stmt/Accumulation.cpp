#include <monlang-LV2/stmt/Accumulation.h>

#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV2/precedence.h>

#include <utils/vec-utils.h>
#include <utils/assert-utils.h>

#define unless(x) if(!(x))

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

Accumulation buildAccumulation(const ProgramSentence& sentence, const context_t* cx) {
    ASSERT (!cx->malformed_stmt && !cx->fallthrough);
    ASSERT (sentence.programWords.size() >= 3);

    unless (holds_word(sentence.programWords[0])) {
        cx->malformed_stmt = "lhs is not a Lvalue";
        return Accumulation(); // stub
    }
    auto word = get_word(sentence.programWords[0]);
    unless (peekLvalue(word)) {
        cx->malformed_stmt = "lhs is not an Lvalue";
        return Accumulation(); // stub
    }
    auto lhs = buildLvalue(word);

    ASSERT (std::holds_alternative<Atom*>(sentence.programWords[1]));
    auto atom = *std::get<Atom*>(sentence.programWords[1]);
    auto optr = atom.value.substr(0, atom.value.size() - 1);

    auto rhs_as_term = extractRhs(sentence);
    unless (rhs_as_term) {
        cx->malformed_stmt = "rhs is an unknown Expression";
        return Accumulation(); // stub
    }
    auto rhs = buildExpression(*rhs_as_term, cx);
    if (cx->fallthrough) {
        cx->malformed_stmt = "rhs is an unknown Expression";
        return Accumulation(); // stub
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
