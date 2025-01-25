#include <monlang-LV2/stmt/ReturnStatement.h>

/* impl only */

#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/ProgramSentence.h>

#include <utils/assert-utils.h>

#define unless(x) if(!(x))

#define SET_TOKEN_FIELDS(returnStmt, sentence) \
    returnStmt._tokenLeadingNewlines = sentence._tokenLeadingNewlines; \
    returnStmt._tokenIndentSpaces = sentence._tokenIndentSpaces; \
    returnStmt._tokenLen = sentence._tokenLen; \
    returnStmt._tokenTrailingNewlines = sentence._tokenTrailingNewlines

#define SET_MALFORMED_TOKEN_FIELDS(malformed, sentence) \
    malformed.val._tokenLeadingNewlines = sentence._tokenLeadingNewlines; \
    malformed.val._tokenIndentSpaces = sentence._tokenIndentSpaces

static Atom AtomConstant(const std::string& val) {
    auto atom = Atom{val};
    atom._tokenLen = val.size();
    return atom;
}

const Atom ReturnStatement::KEYWORD = AtomConstant("return");

bool peekReturnStatement(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 1) {
        return false;
    }

    auto pw = sentence.programWords[0];
    unless (std::holds_alternative<Atom*>(pw)) {
        return false;
    }

    auto atom = *std::get<Atom*>(pw);
    return atom.value == ReturnStatement::KEYWORD.value;
}

static ProgramSentence consumeSentence(LV1::Program&);

// where 'value' are the [1], [2], .. words from the sentence
// ..returns empty opt if any non-word
static std::optional<Term> extractValue(const ProgramSentence&);

MayFail<MayFail_<ReturnStatement>> consumeReturnStatement(LV1::Program& prog) {
    auto sentence = consumeSentence(prog);
    ASSERT (sentence.programWords.size() >= 1);


    if (sentence.programWords.size() == 1) {
        auto returnStmt = MayFail_<ReturnStatement>{}; // std::nullopt
        SET_TOKEN_FIELDS(returnStmt, /*from*/ sentence);
        return returnStmt;
    }


    auto value_as_term = extractValue(sentence);
    unless (value_as_term) {
        auto malformed = Malformed(MayFail_<ReturnStatement>(StubExpression_()), ERR(251));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    auto value = buildExpression(*value_as_term);
    if (value.has_error()) {
        auto malformed = Malformed(MayFail_<ReturnStatement>(value), ERR(252));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }

    auto returnStmt = MayFail_<ReturnStatement>{value};
    SET_TOKEN_FIELDS(returnStmt, /*from*/ sentence);
    return returnStmt;
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

ReturnStatement::ReturnStatement(const std::optional<Expression>& value)
        : value(value){}

MayFail_<ReturnStatement>::MayFail_(const std::optional<MayFail<Expression_>>& value)
        : value(value){}

MayFail_<ReturnStatement>::MayFail_(const ReturnStatement& returnStmt) {
    auto value = std::optional<MayFail<Expression_>>();
    if (returnStmt.value) {
        value = wrap_expr(*returnStmt.value);
    }
    this->value = value;

    this->_tokenLeadingNewlines = returnStmt._tokenLeadingNewlines;
    this->_tokenIndentSpaces = returnStmt._tokenIndentSpaces;
    this->_tokenLen = returnStmt._tokenLen;
    this->_tokenTrailingNewlines = returnStmt._tokenTrailingNewlines;
}

MayFail_<ReturnStatement>::operator ReturnStatement() const {
    auto value = std::optional<Expression>();
    if (this->value) {
        value = unwrap_expr(this->value->value());
    }
    auto returnStmt = ReturnStatement{value};

    returnStmt._tokenLeadingNewlines = this->_tokenLeadingNewlines;
    returnStmt._tokenIndentSpaces = this->_tokenIndentSpaces;
    returnStmt._tokenLen = this->_tokenLen;
    returnStmt._tokenTrailingNewlines = this->_tokenTrailingNewlines;

    return returnStmt;
}
