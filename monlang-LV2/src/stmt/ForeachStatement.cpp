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
        return Malformed(MayFail_<ForeachStatement>{StubExpression_(), MayFail_<BlockExpression>()}, ERR(321));
    }


    auto iterable_as_term = extractIterable(sentence);
    unless (iterable_as_term) {
        return Malformed(MayFail_<ForeachStatement>{StubExpression_(), MayFail_<BlockExpression>()}, ERR(322));
    }
    auto iterable = buildExpression(*iterable_as_term);
    if (iterable.has_error()) {
        return Malformed(MayFail_<ForeachStatement>{iterable, MayFail_<BlockExpression>()}, ERR(323));
    }


    auto pw = sentence.programWords.back();
    unless (holds_word(pw)) {
        return Malformed(MayFail_<ForeachStatement>{iterable, MayFail_<BlockExpression>()}, ERR(324));
    }
    auto word = get_word(pw);
    // TODO: if not peek block expr => new err
    auto block = buildBlockExpression(word);
    if (block.has_error()) {
        return Malformed(MayFail_<ForeachStatement>{iterable, block}, ERR(325));
    }
    //TODO: if block is a oneline => new err

    return MayFail_<ForeachStatement>{iterable, block};
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
