#include <monlang-LV2/stmt/WhileStatement.h>

#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/SquareBracketsTerm.h>
/* require knowing all words for token_len() */
#include <monlang-LV1/ast/Quotation.h>
#include <monlang-LV1/ast/ParenthesesGroup.h>
#include <monlang-LV1/ast/SquareBracketsGroup.h>
#include <monlang-LV1/ast/CurlyBracketsGroup.h>
#include <monlang-LV1/ast/PostfixParenthesesGroup.h>
#include <monlang-LV1/ast/PostfixSquareBracketsGroup.h>
#include <monlang-LV1/ast/Association.h>

#include <utils/assert-utils.h>
#include <utils/stub-ctor.h>

#define unless(x) if(!(x))

#define SET_TOKEN_FIELDS(whileStmt, sentence) \
    whileStmt._tokenLeadingNewlines = sentence._tokenLeadingNewlines; \
    whileStmt._tokenIndentSpaces = sentence._tokenIndentSpaces; \
    whileStmt._tokenLen = sentence._tokenLen; \
    whileStmt._tokenTrailingNewlines = sentence._tokenTrailingNewlines

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

static Atom AtomConstant(const std::string& val) {
    auto atom = Atom{val};
    atom._tokenLen = val.size();
    return atom;
}

const Atom WhileStatement::WHILE_KEYWORD = AtomConstant("while");
const Atom WhileStatement::UNTIL_KEYWORD = AtomConstant("until");

const Atom C_DoStatement::KEYWORD = AtomConstant("do");
const Atom C_WhileStatement::WHILE_KEYWORD = WhileStatement::WHILE_KEYWORD;
const Atom C_WhileStatement::UNTIL_KEYWORD = WhileStatement::UNTIL_KEYWORD;

bool peekWhileStatement(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 1) {
        return false;
    }
    unless (std::holds_alternative<Atom*>(sentence.programWords[0])) {
        return false;
    }
    auto atom = *std::get<Atom*>(sentence.programWords[0]);
    return atom.value == WhileStatement::WHILE_KEYWORD.value
        || atom.value == WhileStatement::UNTIL_KEYWORD.value;
}

bool peekDoWhileStatement(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 1) {
        return false;
    }
    unless (std::holds_alternative<Atom*>(sentence.programWords[0])) {
        return false;
    }
    auto atom = *std::get<Atom*>(sentence.programWords[0]);
    return atom.value == C_DoStatement::KEYWORD.value;
}

static ProgramSentence consumeSentence(LV1::Program&);

MayFail<MayFail_<WhileStatement>> consumeWhileStatement(LV1::Program& prog) {
    auto sentence = consumeSentence(prog);


    /* while/until keyword */
    ASSERT (std::holds_alternative<Atom*>(sentence.programWords[0]));
    auto atom = *std::get<Atom*>(sentence.programWords[0]);
    auto until_loop = atom.value == WhileStatement::UNTIL_KEYWORD.value;


    /* while condition */
    unless (sentence.programWords.size() >= 2) {
        auto malformed = Malformed(MayFail_<WhileStatement>{StubExpression_(), STUB(MayFail_<BlockExpression>), until_loop}, ERR(331));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    auto pw = sentence.programWords[1];
    unless (std::holds_alternative<SquareBracketsTerm*>(pw)) {
        auto error = ERR(332);
        SET_NTH_WORD_ERR_OFFSET(error, 2);
        auto malformed = Malformed(MayFail_<WhileStatement>{StubExpression_(), STUB(MayFail_<BlockExpression>), until_loop}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    auto sbt = *std::get<SquareBracketsTerm*>(pw);
    auto condition = buildExpression(sbt.term);
    if (condition.has_error()) {
        auto malformed = Malformed(MayFail_<WhileStatement>{condition, STUB(MayFail_<BlockExpression>), until_loop}, ERR(333));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }


    /* while block */
    unless (sentence.programWords.size() >= 3) {
        auto malformed = Malformed(MayFail_<WhileStatement>{condition, STUB(MayFail_<BlockExpression>), until_loop}, ERR(334));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    auto block_as_pw = sentence.programWords[2];
    unless (holds_word(block_as_pw)) {
        auto error = ERR(335);
        SET_NTH_WORD_ERR_OFFSET(error, 3);
        auto malformed = Malformed(MayFail_<WhileStatement>{condition, STUB(MayFail_<BlockExpression>), until_loop}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    auto word = get_word(block_as_pw);
    unless (peekBlockExpression(word)) {
        auto error = ERR(338);
        SET_NTH_WORD_ERR_OFFSET(error, 3);
        auto malformed = Malformed(MayFail_<WhileStatement>{condition, STUB(MayFail_<BlockExpression>), until_loop}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    auto block = buildBlockExpression(word);
    if (block.val._dollars) {
        auto error = ERR(337);
        SET_NTH_WORD_ERR_OFFSET(error, 3);
        // NOTE: we construct from a stub block expression (in case the block happens to be malformed as well)
        auto malformed = Malformed(MayFail_<WhileStatement>{condition, STUB(MayFail_<BlockExpression>), until_loop}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    if (block.val._oneline) {
        auto error = ERR(339);
        SET_NTH_WORD_ERR_OFFSET(error, 3);
        // NOTE: we construct from a stub block expression (in case the block happens to be malformed as well)
        auto malformed = Malformed(MayFail_<WhileStatement>{condition, STUB(MayFail_<BlockExpression>), until_loop}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    if (block.has_error()) {
        auto malformed = Malformed(MayFail_<WhileStatement>{condition, block, until_loop}, ERR(336));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }

    // NOTE: additional words are now checked afterward (if warning enabled)
    // ;


    auto whileStmt = MayFail_<WhileStatement>{condition, block, until_loop};
    SET_TOKEN_FIELDS(whileStmt, /*from*/sentence);
    return whileStmt;
}

static MayFail<MayFail_<C_DoStatement>> consumeC_DoStatement(LV1::Program&);
static MayFail<MayFail_<C_WhileStatement>> consumeC_WhileStatement(LV1::Program&);

MayFail<MayFail_<DoWhileStatement>> consumeDoWhileStatement(LV1::Program& prog) {
    auto doStmt = consumeC_DoStatement(prog);
    if (doStmt.has_error()) {
        return Malformed(MayFail_<DoWhileStatement>{doStmt, STUB(MayFail_<C_WhileStatement>)}, ERR(341));
    }

    if (prog.sentences.empty() || !peekWhileStatement(prog.sentences[0])) {
        return Malformed(MayFail_<DoWhileStatement>{doStmt, STUB(MayFail_<C_WhileStatement>)}, ERR(342));
    }

    auto whileStmt = consumeC_WhileStatement(prog);
    if (whileStmt.has_error()) {
        return Malformed(MayFail_<DoWhileStatement>{doStmt, whileStmt}, ERR(343));
    }

    return MayFail_<DoWhileStatement>{doStmt, whileStmt};
}

static MayFail<MayFail_<C_DoStatement>> consumeC_DoStatement(LV1::Program& prog) {
    auto sentence = consumeSentence(prog);

    unless (sentence.programWords.size() >= 2) {
        auto malformed = Malformed(MayFail_<C_DoStatement>{STUB(MayFail_<BlockExpression>)}, ERR(351));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    auto block_as_pw = sentence.programWords[1];
    unless (holds_word(block_as_pw)) {
        auto error = ERR(352);
        SET_NTH_WORD_ERR_OFFSET(error, 2);
        auto malformed = Malformed(MayFail_<C_DoStatement>{STUB(MayFail_<BlockExpression>)}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    auto word = get_word(block_as_pw);
    unless (peekBlockExpression(word)) {
        auto error = ERR(353);
        SET_NTH_WORD_ERR_OFFSET(error, 2);
        auto malformed = Malformed(MayFail_<C_DoStatement>{STUB(MayFail_<BlockExpression>)}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    auto block = buildBlockExpression(word);
    if (block.val._dollars) {
        auto error = ERR(356);
        SET_NTH_WORD_ERR_OFFSET(error, 2);
        // NOTE: we construct from a stub block expression (in case the block happens to be malformed as well)
        auto malformed = Malformed(MayFail_<C_DoStatement>{STUB(MayFail_<BlockExpression>)}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    if (block.val._oneline) {
        auto error = ERR(355);
        SET_NTH_WORD_ERR_OFFSET(error, 2);
        // NOTE: we construct from a stub block expression (in case the block happens to be malformed as well)
        auto malformed = Malformed(MayFail_<C_DoStatement>{STUB(MayFail_<BlockExpression>)}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    if (block.has_error()) {
        auto malformed = Malformed(MayFail_<C_DoStatement>{block}, ERR(354));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }

    // NOTE: additional words are now checked afterward (if warning enabled)
    // ;

    auto c_doStmt = MayFail_<C_DoStatement>{block};
    SET_TOKEN_FIELDS(c_doStmt, /*from*/sentence);
    return c_doStmt;
}

static MayFail<MayFail_<C_WhileStatement>> consumeC_WhileStatement(LV1::Program& prog) {
    auto sentence = consumeSentence(prog);

    /* while/until keyword */
    ASSERT (std::holds_alternative<Atom*>(sentence.programWords[0]));
    auto atom = *std::get<Atom*>(sentence.programWords[0]);
    auto until_loop = atom.value == C_WhileStatement::UNTIL_KEYWORD.value;


    /* while condition */
    unless (sentence.programWords.size() >= 2) {
        auto malformed = Malformed(MayFail_<C_WhileStatement>{StubExpression_(), until_loop}, ERR(361));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    auto pw = sentence.programWords[1];
    unless (std::holds_alternative<SquareBracketsTerm*>(pw)) {
        auto error = ERR(362);
        SET_NTH_WORD_ERR_OFFSET(error, 2);
        auto malformed = Malformed(MayFail_<C_WhileStatement>{StubExpression_(), until_loop}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    auto sbt = *std::get<SquareBracketsTerm*>(pw);
    auto condition = buildExpression(sbt.term);
    if (condition.has_error()) {
        auto malformed = Malformed(MayFail_<C_WhileStatement>{condition, until_loop}, ERR(363));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }

    // NOTE: additional words are now checked afterward (if warning enabled)
    // ;

    auto c_whileStmt = MayFail_<C_WhileStatement>{condition, until_loop};
    SET_TOKEN_FIELDS(c_whileStmt, /*from*/sentence);
    return c_whileStmt;
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

///////////////////////////////////////////////////////////
// NOW, STRUCT CTORS AND CASTS
///////////////////////////////////////////////////////////

/// WhileStatement

WhileStatement::WhileStatement(const Expression& condition, const WhileBlock& block, bool until_loop)
        : condition(condition), block(block), until_loop(until_loop){}

MayFail_<WhileStatement>::MayFail_(const MayFail<Expression_>& condition, const MayFail<MayFail_<WhileBlock>>& block, bool until_loop)
        : condition(condition), block(block), until_loop(until_loop){}

MayFail_<WhileStatement>::MayFail_(const WhileStatement& whileStmt) {
    this->condition = wrap_expr(whileStmt.condition);
    this->block = wrap(whileStmt.block);
    this->until_loop = whileStmt.until_loop;

    this->_tokenLeadingNewlines = whileStmt._tokenLeadingNewlines;
    this->_tokenIndentSpaces = whileStmt._tokenIndentSpaces;
    this->_tokenLen = whileStmt._tokenLen;
    this->_tokenTrailingNewlines = whileStmt._tokenTrailingNewlines;
}

MayFail_<WhileStatement>::operator WhileStatement() const {
    auto condition = unwrap_expr(this->condition.value());
    auto block = unwrap(this->block.value());
    auto until_loop = this->until_loop;
    auto whileStmt = WhileStatement{condition, block, until_loop};

    whileStmt._tokenLeadingNewlines = this->_tokenLeadingNewlines;
    whileStmt._tokenIndentSpaces = this->_tokenIndentSpaces;
    whileStmt._tokenLen = this->_tokenLen;
    whileStmt._tokenTrailingNewlines = this->_tokenTrailingNewlines;

    return whileStmt;
}

/// C_DoStatement

C_DoStatement::C_DoStatement(const WhileBlock& block)
        : block(block){}


MayFail_<C_DoStatement>::MayFail_(const MayFail<MayFail_<WhileBlock>>& block)
        : block(block){}

MayFail_<C_DoStatement>::MayFail_(const C_DoStatement& c_doStmt) {
    this->block = wrap(c_doStmt.block);

    this->_tokenLeadingNewlines = c_doStmt._tokenLeadingNewlines;
    this->_tokenIndentSpaces = c_doStmt._tokenIndentSpaces;
    this->_tokenLen = c_doStmt._tokenLen;
    this->_tokenTrailingNewlines = c_doStmt._tokenTrailingNewlines;
}

MayFail_<C_DoStatement>::operator C_DoStatement() const {
    auto block = unwrap(this->block.value());
    auto c_doStmt = C_DoStatement{block};

    c_doStmt._tokenLeadingNewlines = this->_tokenLeadingNewlines;
    c_doStmt._tokenIndentSpaces = this->_tokenIndentSpaces;
    c_doStmt._tokenLen = this->_tokenLen;
    c_doStmt._tokenTrailingNewlines = this->_tokenTrailingNewlines;

    return c_doStmt;
}

/// C_WhileStatement

C_WhileStatement::C_WhileStatement(const Expression& condition, bool until_loop)
        : condition(condition), until_loop(until_loop){}

MayFail_<C_WhileStatement>::MayFail_(_dummy_stub)
        : _stub(true){}

MayFail_<C_WhileStatement>::MayFail_(const MayFail<Expression_>& condition, bool until_loop)
        : condition(condition), until_loop(until_loop){}

MayFail_<C_WhileStatement>::MayFail_(const C_WhileStatement& c_whileStmt) {
    this->condition = wrap_expr(c_whileStmt.condition);
    this->until_loop = c_whileStmt.until_loop;

    this->_tokenLeadingNewlines = c_whileStmt._tokenLeadingNewlines;
    this->_tokenIndentSpaces = c_whileStmt._tokenIndentSpaces;
    this->_tokenLen = c_whileStmt._tokenLen;
    this->_tokenTrailingNewlines = c_whileStmt._tokenTrailingNewlines;
}

MayFail_<C_WhileStatement>::operator C_WhileStatement() const {
    auto condition = unwrap_expr(this->condition.value());
    auto until_loop = this->until_loop;
    auto c_whileStmt = C_WhileStatement{condition, until_loop};

    c_whileStmt._tokenLeadingNewlines = this->_tokenLeadingNewlines;
    c_whileStmt._tokenIndentSpaces = this->_tokenIndentSpaces;
    c_whileStmt._tokenLen = this->_tokenLen;
    c_whileStmt._tokenTrailingNewlines = this->_tokenTrailingNewlines;

    return c_whileStmt;
}

/// DoWhileStatement

MayFail_<DoWhileStatement>::MayFail_(const MayFail<MayFail_<C_DoStatement>>& doStmt, const MayFail<MayFail_<C_WhileStatement>>& whileStmt)
        : doStmt(doStmt), whileStmt(whileStmt){}

MayFail_<DoWhileStatement>::MayFail_(const DoWhileStatement& doWhileStmt) {
    this->doStmt = wrap(doWhileStmt.doStmt);
    this->whileStmt = wrap(doWhileStmt.whileStmt);
}

MayFail_<DoWhileStatement>::operator DoWhileStatement() const {
    auto doStmt = unwrap(this->doStmt.value());
    auto whileStmt = unwrap(this->whileStmt.value());
    
    return DoWhileStatement{doStmt, whileStmt};
}
