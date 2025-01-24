#include <monlang-LV2/stmt/LetStatement.h>

/* impl only */

#include <monlang-LV1/ast/Atom.h>

#include <utils/assert-utils.h>

#define unless(x) if(!(x))

bool peekLetStatement(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 1) {
        return false;
    }

    auto pw = sentence.programWords[0];
    unless (std::holds_alternative<Atom*>(pw)) {
        return false;
    }

    auto atom = *std::get<Atom*>(pw);
    return atom.value == "let";
}

static ProgramSentence consumeSentence(LV1::Program&);

// where 'value' are the [2], [3], .. words from the sentence
// ..returns empty opt if any non-word
static std::optional<Term> extractValue(const ProgramSentence&);

MayFail<MayFail_<LetStatement>> consumeLetStatement(LV1::Program& prog) {
    auto sentence = consumeSentence(prog);


    unless (sentence.programWords.size() >= 2) {
        return Malformed(MayFail_<LetStatement>{identifier_t(), StubExpression_()}, ERR(231));
    }
    auto word = get_word(sentence.programWords[1]);
    unless (std::holds_alternative<Atom*>(word)) {
        return Malformed(MayFail_<LetStatement>{identifier_t(), StubExpression_()}, ERR(232));
    }
    auto atom = *std::get<Atom*>(word);
    auto identifier = atom.value;


    unless (sentence.programWords.size() >= 3) {
        return Malformed(MayFail_<LetStatement>{identifier, StubExpression_()}, ERR(233));
    }
    auto value_as_term = extractValue(sentence);
    unless (value_as_term) {
        return Malformed(MayFail_<LetStatement>{identifier, StubExpression_()}, ERR(234));
    }
    auto value = buildExpression(*value_as_term);
    if (value.has_error()) {
        return Malformed(MayFail_<LetStatement>{identifier, value}, ERR(235));
    }


    return MayFail_<LetStatement>{identifier, value};
}

static ProgramSentence consumeSentence(LV1::Program& prog) {
    ASSERT (prog.sentences.size() > 0);
    auto res = prog.sentences[0];
    prog.sentences = std::vector(
        prog.sentences.begin() + 1,
        prog.sentences.end()
    );
    return res;
}

static std::optional<Term> extractValue(const ProgramSentence& sentence) {
    ASSERT (sentence.programWords.size() >= 3);
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

MayFail_<LetStatement>::MayFail_(identifier_t identifier, MayFail<Expression_> value)
        : identifier(identifier), value(value){}

MayFail_<LetStatement>::MayFail_(LetStatement letStmt) {
    this->identifier = letStmt.identifier;
    this->value = wrap_expr(letStmt.value);
}

MayFail_<LetStatement>::operator LetStatement() const {
    auto value = unwrap_expr(this->value.value());
    return LetStatement{this->identifier, value};
}
