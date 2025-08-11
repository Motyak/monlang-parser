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

MayFail<MayFail_<LetStatement>> consumeLetStatement(LV1::Program& prog) {
    auto sentence = consumeSentence(prog);


    unless (sentence.programWords.size() >= 2) {
        auto malformed = Malformed(MayFail_<LetStatement>{Symbol(), STUB(Lvalue_)}, ERR(231));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    unless (holds_word(sentence.programWords[1])) {
        auto error = ERR(232);
        SET_NTH_WORD_ERR_OFFSET(error, /*nth*/2);
        auto malformed = Malformed(MayFail_<LetStatement>{Symbol(), STUB(Lvalue_)}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    auto aliasWord = get_word(sentence.programWords[1]);
    auto aliasExpr = buildExpression((Term)aliasWord);
    unless (std::holds_alternative<Symbol*>(aliasExpr.val)) {
        auto error = ERR(233);
        SET_NTH_WORD_ERR_OFFSET(error, /*nth*/2);
        auto malformed = Malformed(MayFail_<LetStatement>{Symbol(), STUB(Lvalue_)}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    auto alias = *std::get<Symbol*>(aliasExpr.val);


    unless (sentence.programWords.size() >= 3) {
        auto malformed = Malformed(MayFail_<LetStatement>{alias, STUB(Lvalue_)}, ERR(234));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    unless (holds_word(sentence.programWords[2])) {
        auto error = ERR(235);
        SET_NTH_WORD_ERR_OFFSET(error, /*nth*/3);
        auto malformed = Malformed(MayFail_<LetStatement>{alias, STUB(Lvalue_)}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    auto variableWord = get_word(sentence.programWords[2]);
    auto variableExpr = buildExpression((Term)variableWord);
    if (!is_lvalue(variableExpr.val)) {
        auto error = ERR(236);
        SET_NTH_WORD_ERR_OFFSET(error, /*nth*/3);
        auto malformed = Malformed(MayFail_<LetStatement>{alias, STUB(Lvalue_)}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    auto variable = mayfail_cast<Lvalue_>(variableExpr);
    if (variable.has_error()) {
        auto error = ERR(237);
        SET_NTH_WORD_ERR_OFFSET(error, /*nth*/3);
        auto malformed = Malformed(MayFail_<LetStatement>{alias, variable}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }


    auto letStmt = MayFail_<LetStatement>{alias, variable};
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

LetStatement::LetStatement(const Symbol& alias, const Lvalue& variable)
        : alias(alias), variable(variable){}

MayFail_<LetStatement>::MayFail_(const Symbol& alias, const MayFail<Lvalue_>& variable)
        : alias(alias), variable(variable){}

MayFail_<LetStatement>::MayFail_(const LetStatement& letStmt)
        : variable(wrap_lvalue(letStmt.variable))
{
    this->alias = letStmt.alias;

    this->_tokenLeadingNewlines = letStmt._tokenLeadingNewlines;
    this->_tokenIndentSpaces = letStmt._tokenIndentSpaces;
    this->_tokenLen = letStmt._tokenLen;
    this->_tokenTrailingNewlines = letStmt._tokenTrailingNewlines;
    this->_tokenId = letStmt._tokenId;
}

MayFail_<LetStatement>::operator LetStatement() const {
    auto alias = this->alias;
    auto variable = unwrap_lvalue(this->variable.value());
    auto letStmt = LetStatement{alias, variable};

    letStmt._tokenLeadingNewlines = this->_tokenLeadingNewlines;
    letStmt._tokenIndentSpaces = this->_tokenIndentSpaces;
    letStmt._tokenLen = this->_tokenLen;
    letStmt._tokenTrailingNewlines = this->_tokenTrailingNewlines;
    letStmt._tokenId = this->_tokenId;

    return letStmt;
}
