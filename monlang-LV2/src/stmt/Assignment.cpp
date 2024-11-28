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

// copy pasted from ExpressionStatement.cpp
static Term toTerm(const ProgramSentence& sentence) {
    std::vector<Word> words;
    for (auto e: sentence.programWords) {
        words.push_back(get_word(e));
    }
    return Term{words};
}

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

    auto rhs_prog_words = std::vector<ProgramWord>(
        sentence.programWords.begin() + 2,
        sentence.programWords.end()
    );
    auto rhs_as_term = toTerm(ProgramSentence{rhs_prog_words});
    auto rhs = buildExpression(rhs_as_term);
    if (cx.fallthrough) {
        cx.malformed_stmt = "rhs is an unknown Expression";
        return Assignment(); // stub
    }

    return Assignment{lhs, rhs};
}
