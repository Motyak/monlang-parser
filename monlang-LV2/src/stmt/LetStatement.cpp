#include <monlang-LV2/stmt/LetStatement.h>

/* impl only */

#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/ProgramSentence.h>
/* require knowing all words for token_len() */
#include <monlang-LV1/ast/token_len.h>

#include <utils/assert-utils.h>

#define unless(x) if(!(x))

#define SET_TOKEN_FIELDS(letStmt, sentence) \
    letStmt._tokenLeadingNewlines = sentence._tokenLeadingNewlines; \
    letStmt._tokenIndentSpaces = sentence._tokenIndentSpaces; \
    letStmt._tokenLen = sentence._tokenLen; \
    letStmt._tokenTrailingNewlines = sentence._tokenTrailingNewlines

#define SET_MALFORMED_TOKEN_FIELDS(malformed, sentence) \
    malformed.val._tokenLeadingNewlines = sentence._tokenLeadingNewlines; \
    malformed.val._tokenIndentSpaces = sentence._tokenIndentSpaces

// sum token len for all words preceding the nth word..
// ..and add it to error offset
#define SET_NTH_WORD_ERR_OFFSET(error, nth) \
    auto err_offset = size_t(0); \
    for (size_t i = 0; i < nth - 1; ++i) { \
        err_offset += token_len(sentence.programWords[i]); \
        err_offset += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE); \
    } \
    error._info["err_offset"] = err_offset


// sum token len for all words preceding the first non-Word..
// ..and add it to error offset
#define SET_NON_WORD_ERR_OFFSET(error) \
    auto err_offset = size_t(0); \
    for (size_t i = 0; i < sentence.programWords.size(); ++i) { \
        unless (holds_word(sentence.programWords[i])) break; \
        err_offset += token_len(sentence.programWords[i]); \
        err_offset += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE); \
    } \
    error._info["err_offset"] = err_offset

const std::string LetStatement::KEYWORD = "let";

bool peekLetStatement(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 1) {
        return false;
    }

    auto pw = sentence.programWords[0];
    unless (std::holds_alternative<Atom*>(pw)) {
        return false;
    }

    auto atom = *std::get<Atom*>(pw);
    return atom.value == LetStatement::KEYWORD;
}

static ProgramSentence consumeSentence(LV1::Program&);

// where 'value' are the [2], [3], .. words from the sentence
// ..returns empty opt if any non-word
static std::optional<Term> extractValue(const ProgramSentence&);

MayFail<MayFail_<LetStatement>> consumeLetStatement(LV1::Program& prog) {
    auto sentence = consumeSentence(prog);


    unless (sentence.programWords.size() >= 2) {
        auto malformed = Malformed(MayFail_<LetStatement>{Symbol(), StubExpression_()}, ERR(231));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    unless (holds_word(sentence.programWords[1])) {
        auto error = ERR(236);
        SET_NTH_WORD_ERR_OFFSET(error, /*nth*/2);
        auto malformed = Malformed(MayFail_<LetStatement>{Symbol(), StubExpression_()}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    auto word = get_word(sentence.programWords[1]);
    auto expr = buildExpression((Term)word);
    unless (std::holds_alternative<Symbol*>(expr.val)) {
        auto error = ERR(232);
        SET_NTH_WORD_ERR_OFFSET(error, /*nth*/2);
        auto malformed = Malformed(MayFail_<LetStatement>{Symbol(), StubExpression_()}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    Symbol name = std::get<Symbol*>(expr.val)->name;


    unless (sentence.programWords.size() >= 3) {
        auto malformed = Malformed(MayFail_<LetStatement>{name, StubExpression_()}, ERR(233));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    auto value_as_term = extractValue(sentence);
    unless (value_as_term) {
        auto error = ERR(234);
        SET_NON_WORD_ERR_OFFSET(error);
        auto malformed = Malformed(MayFail_<LetStatement>{name, StubExpression_()}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    auto value = buildExpression(*value_as_term);
    if (value.has_error()) {
        auto malformed = Malformed(MayFail_<LetStatement>{name, value}, ERR(235));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }

    auto letStmt = MayFail_<LetStatement>{name, value};
    SET_TOKEN_FIELDS(letStmt, /*from*/ sentence);
    return letStmt;
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

    size_t wordsTokenLen = 0;
    std::vector<Word> words;
    for (auto e: rhs_as_sentence) {
        unless (holds_word(e)) {
            return {};
        }
        auto word = get_word(e);
        words.push_back(get_word(e));
        wordsTokenLen += token_len(word);
    }

    auto term = Term{words};
    term._tokenLen = wordsTokenLen
            + (words.size() - 1) * sequenceLen(Term::CONTINUATOR_SEQUENCE);
    return term;
}

LetStatement::LetStatement(const Symbol& label, const Expression& value)
        : label(label), value(value){}

MayFail_<LetStatement>::MayFail_(const Symbol& label, const MayFail<Expression_>& value)
        : label(label), value(value){}

MayFail_<LetStatement>::MayFail_(const LetStatement& letStmt) {
    this->label = letStmt.label;
    this->value = wrap_expr(letStmt.value);

    this->_tokenLeadingNewlines = letStmt._tokenLeadingNewlines;
    this->_tokenIndentSpaces = letStmt._tokenIndentSpaces;
    this->_tokenLen = letStmt._tokenLen;
    this->_tokenTrailingNewlines = letStmt._tokenTrailingNewlines;
}

MayFail_<LetStatement>::operator LetStatement() const {
    auto label = this->label;
    auto value = unwrap_expr(this->value.value());
    auto letStmt = LetStatement{label, value};

    letStmt._tokenLeadingNewlines = this->_tokenLeadingNewlines;
    letStmt._tokenIndentSpaces = this->_tokenIndentSpaces;
    letStmt._tokenLen = this->_tokenLen;
    letStmt._tokenTrailingNewlines = this->_tokenTrailingNewlines;

    return letStmt;
}
