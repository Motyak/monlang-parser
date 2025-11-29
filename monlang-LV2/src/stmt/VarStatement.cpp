#include <monlang-LV2/stmt/VarStatement.h>

/* impl only */

#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/ProgramSentence.h>
/* require knowing all words for token_len() */
#include <monlang-LV1/ast/token_len.h>

#include <utils/assert-utils.h>

#define unless(x) if(!(x))

#define SET_TOKEN_FIELDS(varStmt, sentence) \
    varStmt._tokenLeadingNewlines = sentence._tokenLeadingNewlines; \
    varStmt._tokenIndentSpaces = sentence._tokenIndentSpaces; \
    varStmt._tokenLen = sentence._tokenLen; \
    varStmt._tokenTrailingNewlines = sentence._tokenTrailingNewlines

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

const std::string VarStatement::KEYWORD = "var";

bool peekVarStatement(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 1) {
        return false;
    }

    auto pw = sentence.programWords[0];
    unless (std::holds_alternative<Atom*>(pw)) {
        return false;
    }

    auto atom = *std::get<Atom*>(pw);
    return atom.value == VarStatement::KEYWORD;
}

static ProgramSentence consumeSentence(LV1::Program&);

// where 'value' are the [2], [3], .. words from the sentence
// ..returns empty opt if any non-word
static std::optional<Term> extractValue(const ProgramSentence&);

MayFail<MayFail_<VarStatement>> consumeVarStatement(LV1::Program& prog) {
    auto sentence = consumeSentence(prog);


    unless (sentence.programWords.size() >= 2) {
        auto malformed = Malformed(MayFail_<VarStatement>{Symbol(), StubExpression_()}, ERR(241));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    unless (holds_word(sentence.programWords[1])) {
        auto error = ERR(242);
        SET_NTH_WORD_ERR_OFFSET(error, /*nth*/2);
        auto malformed = Malformed(MayFail_<VarStatement>{Symbol(), StubExpression_()}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    auto word = get_word(sentence.programWords[1]);
    auto is_an_atom = std::holds_alternative<Atom*>(word);
    auto expr = buildExpression((Term)word);
    unless (std::holds_alternative<Symbol*>(expr.val)) {
        auto error = ERR(243);
        SET_NTH_WORD_ERR_OFFSET(error, /*nth*/2);
        auto malformed = Malformed(MayFail_<VarStatement>{Symbol(), StubExpression_()}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    auto name = *std::get<Symbol*>(expr.val);

    unless (is_an_atom) {
        auto error = ERR(247);
        SET_NTH_WORD_ERR_OFFSET(error, /*nth*/2);
        auto malformed = Malformed(MayFail_<VarStatement>{name, StubExpression_()}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }


    unless (sentence.programWords.size() >= 3) {
        auto malformed = Malformed(MayFail_<VarStatement>{name, StubExpression_()}, ERR(244));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    auto value_as_term = extractValue(sentence);
    unless (value_as_term) {
        auto error = ERR(245);
        SET_NON_WORD_ERR_OFFSET(error);
        auto malformed = Malformed(MayFail_<VarStatement>{name, StubExpression_()}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    auto value = buildExpression(*value_as_term);
    if (value.has_error()) {
        auto malformed = Malformed(MayFail_<VarStatement>{name, value}, ERR(246));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }

    auto varStmt = MayFail_<VarStatement>{name, value};
    SET_TOKEN_FIELDS(varStmt, /*from*/ sentence);
    return varStmt;
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

VarStatement::VarStatement(const Symbol& variable, const Expression& value)
        : variable(variable), value(value){}

MayFail_<VarStatement>::MayFail_(const Symbol& variable, const MayFail<Expression_>& value)
        : variable(variable), value(value){}

MayFail_<VarStatement>::MayFail_(const VarStatement& varStmt) {
    this->variable = varStmt.variable;
    this->value = wrap_expr(varStmt.value);

    this->_tokenLeadingNewlines = varStmt._tokenLeadingNewlines;
    this->_tokenIndentSpaces = varStmt._tokenIndentSpaces;
    this->_tokenLen = varStmt._tokenLen;
    this->_tokenTrailingNewlines = varStmt._tokenTrailingNewlines;
    this->_tokenId = varStmt._tokenId;
}

MayFail_<VarStatement>::operator VarStatement() const {
    auto variable = this->variable;
    auto value = unwrap_expr(this->value.value());
    auto varStmt = VarStatement{variable, value};

    varStmt._tokenLeadingNewlines = this->_tokenLeadingNewlines;
    varStmt._tokenIndentSpaces = this->_tokenIndentSpaces;
    varStmt._tokenLen = this->_tokenLen;
    varStmt._tokenTrailingNewlines = this->_tokenTrailingNewlines;
    varStmt._tokenId = this->_tokenId;

    return varStmt;
}
