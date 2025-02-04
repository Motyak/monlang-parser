#include <monlang-LV2/stmt/WhileStatement.h>

#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/SquareBracketsTerm.h>

#include <utils/assert-utils.h>

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

const Atom WhileStatement::WHILE_KEYWORD = AtomConstant("while");
const Atom WhileStatement::UNTIL_KEYWORD = AtomConstant("until");

const Atom DoWhileStatement::DO_KEYWORD = AtomConstant("do");
const Atom DoWhileStatement::WHILE_KEYWORD = WhileStatement::WHILE_KEYWORD;
const Atom DoWhileStatement::UNTIL_KEYWORD = WhileStatement::UNTIL_KEYWORD;

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
    return atom.value == DoWhileStatement::DO_KEYWORD.value;
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
        auto malformed = Malformed(MayFail_<WhileStatement>{StubExpression_(), MayFail_<BlockExpression>(), until_loop}, ERR(331));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    auto pw = sentence.programWords[1];
    unless (std::holds_alternative<SquareBracketsTerm*>(pw)) {
        auto malformed = Malformed(MayFail_<WhileStatement>{StubExpression_(), MayFail_<BlockExpression>(), until_loop}, ERR(332));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    auto sbt = *std::get<SquareBracketsTerm*>(pw);
    auto condition = buildExpression(sbt.term);
    if (condition.has_error()) {
        auto malformed = Malformed(MayFail_<WhileStatement>{condition, MayFail_<BlockExpression>(), until_loop}, ERR(333));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }


    /* while block */
    unless (sentence.programWords.size() >= 3) {
        auto malformed = Malformed(MayFail_<WhileStatement>{condition, MayFail_<BlockExpression>(), until_loop}, ERR(334));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    auto block_as_pw = sentence.programWords[2];
    unless (holds_word(block_as_pw)) {
        auto malformed = Malformed(MayFail_<WhileStatement>{condition, MayFail_<BlockExpression>(), until_loop}, ERR(335));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    auto word = get_word(block_as_pw);
    unless (peekBlockExpression(word)) {
        auto malformed = Malformed(MayFail_<WhileStatement>{condition, MayFail_<BlockExpression>(), until_loop}, ERR(338));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    auto block = buildBlockExpression(word);
    if (block.has_error()) {
        auto malformed = Malformed(MayFail_<WhileStatement>{condition, block, until_loop}, ERR(336));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    if (block.val._oneline) {
        auto malformed = Malformed(MayFail_<WhileStatement>{condition, block, until_loop}, ERR(339));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }

    // NOTE: additional words are now checked afterward (if warning enabled)
    // ;


    auto whileStmt = MayFail_<WhileStatement>{condition, block, until_loop};
    SET_TOKEN_FIELDS(whileStmt, /*from*/sentence);
    return whileStmt;
}

MayFail<MayFail_<DoWhileStatement>> consumeDoWhileStatement(LV1::Program& prog) {
    auto currSentence = consumeSentence(prog);

    /* do block */
    unless (currSentence.programWords.size() >= 2) {
        auto malformed = Malformed(MayFail_<DoWhileStatement>{MayFail_<BlockExpression>(), StubExpression_(), bool()}, ERR(341));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/currSentence);
        return malformed;
    }
    auto block_as_pw = currSentence.programWords[1];
    unless (holds_word(block_as_pw)) {
        auto malformed = Malformed(MayFail_<DoWhileStatement>{MayFail_<BlockExpression>(), StubExpression_(), bool()}, ERR(342));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/currSentence);
        return malformed;
    }
    auto word = get_word(block_as_pw);
    unless (peekBlockExpression(word)) {
        auto malformed = Malformed(MayFail_<DoWhileStatement>{MayFail_<BlockExpression>(), StubExpression_(), bool()}, ERR(349));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/currSentence);
        return malformed;
    }
    auto block = buildBlockExpression(word);
    if (block.has_error()) {
        auto malformed = Malformed(MayFail_<DoWhileStatement>{block, StubExpression_(), bool()}, ERR(343));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/currSentence);
        return malformed;
    }
    if (block.val._oneline) {
        auto malformed = Malformed(MayFail_<DoWhileStatement>{block, StubExpression_(), bool()}, ERR(340));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/currSentence);
        return malformed;
    }

    // NOTE: additional words are now checked afterward (if warning enabled)
    // ;


    currSentence = consumeSentence(prog);

    /* while/until keyword */
    ASSERT (std::holds_alternative<Atom*>(currSentence.programWords[0]));
    auto atom = *std::get<Atom*>(currSentence.programWords[0]);
    auto until_loop = atom.value == DoWhileStatement::UNTIL_KEYWORD.value;


    /* while condition */
    unless (currSentence.programWords.size() >= 2) {
        auto malformed = Malformed(MayFail_<DoWhileStatement>{block, StubExpression_(), until_loop}, ERR(345));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/currSentence);
        return malformed;
    }
    auto pw = currSentence.programWords[1];
    unless (std::holds_alternative<SquareBracketsTerm*>(pw)) {
        auto malformed = Malformed(MayFail_<DoWhileStatement>{block, StubExpression_(), until_loop}, ERR(346));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/currSentence);
        return malformed;
    }
    auto sbt = *std::get<SquareBracketsTerm*>(pw);
    auto condition = buildExpression(sbt.term);
    if (condition.has_error()) {
        auto malformed = Malformed(MayFail_<DoWhileStatement>{block, condition, until_loop}, ERR(347));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/currSentence);
        return malformed;
    }

    // NOTE: additional words are now checked afterward (if warning enabled)
    // ;


    auto doWhileStmt = MayFail_<DoWhileStatement>{block, condition, until_loop};
    SET_TOKEN_FIELDS(doWhileStmt, /*from*/currSentence);
    return doWhileStmt;
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

DoWhileStatement::DoWhileStatement(const WhileBlock& block, const Expression& condition, bool until_loop)
        : block(block), condition(condition), until_loop(until_loop){}

MayFail_<DoWhileStatement>::MayFail_(const MayFail<MayFail_<WhileBlock>>& block, const MayFail<Expression_>& condition, bool until_loop)
        : block(block), condition(condition), until_loop(until_loop){}

MayFail_<DoWhileStatement>::MayFail_(const DoWhileStatement& doWhileStmt) {
    this->block = wrap(doWhileStmt.block);
    this->condition = wrap_expr(doWhileStmt.condition);
    this->until_loop = doWhileStmt.until_loop;

    this->_tokenLeadingNewlines = doWhileStmt._tokenLeadingNewlines;
    this->_tokenIndentSpaces = doWhileStmt._tokenIndentSpaces;
    this->_tokenLen = doWhileStmt._tokenLen;
    this->_tokenTrailingNewlines = doWhileStmt._tokenTrailingNewlines;
}

MayFail_<DoWhileStatement>::operator DoWhileStatement() const {
    auto block = unwrap(this->block.value());
    auto condition = unwrap_expr(this->condition.value());
    auto until_loop = this->until_loop;
    auto doWhileStmt = DoWhileStatement{block, condition, until_loop};

    doWhileStmt._tokenLeadingNewlines = this->_tokenLeadingNewlines;
    doWhileStmt._tokenIndentSpaces = this->_tokenIndentSpaces;
    doWhileStmt._tokenLen = this->_tokenLen;
    doWhileStmt._tokenTrailingNewlines = this->_tokenTrailingNewlines;

    return doWhileStmt;
}
