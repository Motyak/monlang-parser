#include <monlang-LV2/stmt/ForeachStatement.h>

/* impl only */

#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/ProgramSentence.h>
/* require knowing all words for token_len() */
#include <monlang-LV1/ast/ParenthesesGroup.h>
#include <monlang-LV1/ast/SquareBracketsGroup.h>
#include <monlang-LV1/ast/CurlyBracketsGroup.h>
#include <monlang-LV1/ast/PostfixParenthesesGroup.h>
#include <monlang-LV1/ast/PostfixSquareBracketsGroup.h>
#include <monlang-LV1/ast/Association.h>
#include <monlang-LV1/ast/SquareBracketsTerm.h>

#include <utils/assert-utils.h>

#define unless(x) if(!(x))

#define SET_TOKEN_FIELDS(foreachStmt, sentence) \
    foreachStmt._tokenLeadingNewlines = sentence._tokenLeadingNewlines; \
    foreachStmt._tokenIndentSpaces = sentence._tokenIndentSpaces; \
    foreachStmt._tokenLen = sentence._tokenLen; \
    foreachStmt._tokenTrailingNewlines = sentence._tokenTrailingNewlines

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

static Atom AtomConstant(const std::string& val) {
    auto atom = Atom{val};
    atom._tokenLen = val.size();
    return atom;
}

const Atom ForeachStatement::KEYWORD = AtomConstant("foreach");

bool peekForeachStatement(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 1) {
        return false;
    }
    unless (std::holds_alternative<Atom*>(sentence.programWords[0])) {
        return false;
    }
    auto atom = *std::get<Atom*>(sentence.programWords[0]);
    return atom.value == ForeachStatement::KEYWORD.value;
}

static ProgramSentence consumeSentence(LV1::Program&);

// returns empty opt if non-word
static std::optional<Term> extractIterable(const ProgramSentence&);

MayFail<MayFail_<ForeachStatement>> consumeForeachStatement(LV1::Program& prog) {
    auto sentence = consumeSentence(prog);
    unless (sentence.programWords.size() >= 3) {
        auto malformed = Malformed(MayFail_<ForeachStatement>{StubExpression_(), MayFail_<BlockExpression>()}, ERR(321));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }


    auto iterable_as_term = extractIterable(sentence);
    unless (iterable_as_term) {
        auto error = ERR(322);
        SET_NON_WORD_ERR_OFFSET(error);
        auto malformed = Malformed(MayFail_<ForeachStatement>{StubExpression_(), MayFail_<BlockExpression>()}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    auto iterable = buildExpression(*iterable_as_term);
    if (iterable.has_error()) {
        auto malformed = Malformed(MayFail_<ForeachStatement>{iterable, MayFail_<BlockExpression>()}, ERR(323));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }


    auto pw = sentence.programWords.back();
    unless (holds_word(pw)) {
        auto error = ERR(324);
        SET_NON_WORD_ERR_OFFSET(error);
        auto malformed = Malformed(MayFail_<ForeachStatement>{iterable, MayFail_<BlockExpression>()}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    auto word = get_word(pw);
    unless (peekBlockExpression(word)) {
        auto error = ERR(326);
        SET_NTH_WORD_ERR_OFFSET(error, sentence.programWords.size());
        auto malformed = Malformed(MayFail_<ForeachStatement>{iterable, MayFail_<BlockExpression>()}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    auto block = buildBlockExpression(word);
    if (block.has_error()) {
        auto malformed = Malformed(MayFail_<ForeachStatement>{iterable, block}, ERR(325));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    if (block.val._oneline) {
        auto error = ERR(327);
        SET_NTH_WORD_ERR_OFFSET(error, sentence.programWords.size());
        auto malformed = Malformed(MayFail_<ForeachStatement>{iterable, block}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }

    auto foreachStmt = MayFail_<ForeachStatement>{iterable, block};
    SET_TOKEN_FIELDS(foreachStmt, /*from*/sentence);
    return foreachStmt;
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

static std::optional<Term> extractIterable(const ProgramSentence& sentence) {
    ASSERT (sentence.programWords.size() >= 3);

    auto iterable_as_sentence = std::vector<ProgramWord>(
        sentence.programWords.begin() + 1,
        sentence.programWords.end() - 1
    );

    size_t wordsTokenLen = 0;
    std::vector<Word> words;
    for (auto e: iterable_as_sentence) {
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

ForeachStatement::ForeachStatement(const Expression& iterable, const ForeachBlock& block)
        : iterable(iterable), block(block){}

MayFail_<ForeachStatement>::MayFail_(const MayFail<Expression_>& iterable, const MayFail<MayFail_<ForeachBlock>>& block)
        : iterable(iterable), block(block){}

MayFail_<ForeachStatement>::MayFail_(const ForeachStatement& foreachStmt) {
    this->iterable = wrap_expr(foreachStmt.iterable);
    this->block = wrap(foreachStmt.block);

    this->_tokenLeadingNewlines = foreachStmt._tokenLeadingNewlines;
    this->_tokenIndentSpaces = foreachStmt._tokenIndentSpaces;
    this->_tokenLen = foreachStmt._tokenLen;
    this->_tokenTrailingNewlines = foreachStmt._tokenTrailingNewlines;
}

MayFail_<ForeachStatement>::operator ForeachStatement() const {
    auto iterable = unwrap_expr(this->iterable.value());
    auto block = unwrap(this->block.value());
    auto foreachStmt = ForeachStatement{iterable, block};

    foreachStmt._tokenLeadingNewlines = this->_tokenLeadingNewlines;
    foreachStmt._tokenIndentSpaces = this->_tokenIndentSpaces;
    foreachStmt._tokenLen = this->_tokenLen;
    foreachStmt._tokenTrailingNewlines = this->_tokenTrailingNewlines;

    return foreachStmt;
}
