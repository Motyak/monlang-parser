#include <monlang-LV2/stmt/ReturnStatement.h>

/* impl only */

#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/ProgramSentence.h>

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

static ProgramSentence consumeSentence(LV1::Program&);

// where 'value' are the [1], [2], .. words from the sentence
// ..returns empty opt if any non-word
static std::optional<Term> extractValue(const ProgramSentence&);

MayFail<MayFail_<ReturnStatement>> consumeReturnStatement(LV1::Program& prog) {
    auto sentence = consumeSentence(prog);
    ASSERT (sentence.programWords.size() >= 1);


    if (sentence.programWords.size() == 1) {
        return MayFail_<ReturnStatement>{}; // std::nullopt
    }


    auto value_as_term = extractValue(sentence);
    unless (value_as_term) {
        return Malformed(MayFail_<ReturnStatement>(Expression_()), ERR(251));
    }
    auto value = buildExpression(*value_as_term);
    if (value.has_error()) {
        return Malformed(MayFail_<ReturnStatement>(value), ERR(252));
    }


    return MayFail_<ReturnStatement>{value};
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
    ASSERT (sentence.programWords.size() >= 2);

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

MayFail_<ReturnStatement>::MayFail_(std::optional<MayFail<Expression_>> value) : value(value){}

MayFail_<ReturnStatement>::MayFail_(ReturnStatement returnStmt) {
    auto value = std::optional<MayFail<Expression_>>();
    if (returnStmt.value) {
        value = wrap_expr(*returnStmt.value);
    }
    this->value = value;
}

MayFail_<ReturnStatement>::operator ReturnStatement() const {
    auto value = std::optional<Expression>();
    if (this->value) {
        value = unwrap_expr(this->value->value());
    }
    return ReturnStatement{value};
}
