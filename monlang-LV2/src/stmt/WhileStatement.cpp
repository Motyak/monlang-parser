#include <monlang-LV2/stmt/WhileStatement.h>

#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/SquareBracketsTerm.h>

#include <utils/assert-utils.h>

#define unless(x) if(!(x))

bool peekWhileStatement(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 1) {
        return false;
    }
    unless (std::holds_alternative<Atom*>(sentence.programWords[0])) {
        return false;
    }
    auto atom = *std::get<Atom*>(sentence.programWords[0]);
    return atom.value == "while"
        || atom.value == "until";
}

bool peekDoWhileStatement(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 1) {
        return false;
    }
    unless (std::holds_alternative<Atom*>(sentence.programWords[0])) {
        return false;
    }
    auto atom = *std::get<Atom*>(sentence.programWords[0]);
    return atom.value == "do";
}

static ProgramSentence consumeSentence(LV1::Program&);

MayFail<MayFail_<WhileStatement>> consumeWhileStatement(LV1::Program& prog) {
    auto sentence = consumeSentence(prog);


    /* while/until keyword */
    ASSERT (std::holds_alternative<Atom*>(sentence.programWords[0]));
    auto atom = *std::get<Atom*>(sentence.programWords[0]);
    auto until_loop = atom.value == "until";


    /* while condition */
    unless (sentence.programWords.size() >= 2) {
        return Malformed(MayFail_<WhileStatement>{StubExpression_(), MayFail_<BlockExpression>(), until_loop}, ERR(331));
    }
    auto pw = sentence.programWords[1];
    unless (std::holds_alternative<SquareBracketsTerm*>(pw)) {
        return Malformed(MayFail_<WhileStatement>{StubExpression_(), MayFail_<BlockExpression>(), until_loop}, ERR(332));
    }
    auto sbt = *std::get<SquareBracketsTerm*>(pw);
    auto condition = buildExpression(sbt.term);
    if (condition.has_error()) {
        return Malformed(MayFail_<WhileStatement>{condition, MayFail_<BlockExpression>(), until_loop}, ERR(333));
    }


    /* while block */
    unless (sentence.programWords.size() >= 3) {
        return Malformed(MayFail_<WhileStatement>{condition, MayFail_<BlockExpression>(), until_loop}, ERR(334));
    }
    auto block_as_pw = sentence.programWords[2];
    unless (holds_word(block_as_pw)) {
        return Malformed(MayFail_<WhileStatement>{condition, MayFail_<BlockExpression>(), until_loop}, ERR(335));
    }
    auto block_as_word = get_word(block_as_pw);
    auto block = buildBlockExpression(block_as_word);
    if (block.has_error()) {
        return Malformed(MayFail_<WhileStatement>{condition, block, until_loop}, ERR(336));
    }


    /* check if additional words */
    if (sentence.programWords.size() > 3) {
        return Malformed(MayFail_<WhileStatement>{condition, block, until_loop}, ERR(337));
    }


    return MayFail_<WhileStatement>{condition, block, until_loop};
}

MayFail<MayFail_<DoWhileStatement>> consumeDoWhileStatement(LV1::Program& prog) {
    auto currSentence = consumeSentence(prog);

    /* while block */
    unless (currSentence.programWords.size() >= 2) {
        return Malformed(MayFail_<DoWhileStatement>{MayFail_<BlockExpression>(), StubExpression_(), false}, ERR(341));
    }
    auto block_as_pw = currSentence.programWords[1];
    unless (holds_word(block_as_pw)) {
        return Malformed(MayFail_<DoWhileStatement>{MayFail_<BlockExpression>(), StubExpression_(), false}, ERR(342));
    }
    auto block_as_word = get_word(block_as_pw);
    auto block = buildBlockExpression(block_as_word);
    if (block.has_error()) {
        return Malformed(MayFail_<DoWhileStatement>{block, StubExpression_(), false}, ERR(343));
    }


    /* check if additional words */
    if (currSentence.programWords.size() > 2) {
        return Malformed(MayFail_<DoWhileStatement>{block, StubExpression_(), false}, ERR(344));
    }


    currSentence = consumeSentence(prog);

    /* while/until keyword */
    ASSERT (std::holds_alternative<Atom*>(currSentence.programWords[0]));
    auto atom = *std::get<Atom*>(currSentence.programWords[0]);
    auto until_loop = atom.value == "until";


    /* while condition */
    unless (currSentence.programWords.size() >= 2) {
        return Malformed(MayFail_<DoWhileStatement>{block, StubExpression_(), until_loop}, ERR(345));
    }
    auto pw = currSentence.programWords[1];
    unless (std::holds_alternative<SquareBracketsTerm*>(pw)) {
        return Malformed(MayFail_<DoWhileStatement>{block, StubExpression_(), until_loop}, ERR(346));
    }
    auto sbt = *std::get<SquareBracketsTerm*>(pw);
    auto condition = buildExpression(sbt.term);
    if (condition.has_error()) {
        return Malformed(MayFail_<DoWhileStatement>{block, condition, until_loop}, ERR(347));
    }


    /* check if additional words */
    if (currSentence.programWords.size() > 2) {
        return Malformed(MayFail_<DoWhileStatement>{block, condition, until_loop}, ERR(348));
    }


    return MayFail_<DoWhileStatement>{block, condition, until_loop};
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

MayFail_<WhileStatement>::MayFail_(MayFail<Expression_> condition, MayFail<MayFail_<WhileBlock>> block, bool until_loop)
        : condition(condition), block(block), until_loop(until_loop){}

MayFail_<WhileStatement>::MayFail_(WhileStatement whileStmt) {
    this->condition = wrap_expr(whileStmt.condition);
    this->block = wrap(whileStmt.block);
    this->until_loop = whileStmt.until_loop;
}

MayFail_<WhileStatement>::operator WhileStatement() const {
    auto condition = unwrap_expr(this->condition.value());
    auto block = unwrap(this->block.value());
    auto until_loop = this->until_loop;
    return WhileStatement{condition, block, until_loop};
}

MayFail_<DoWhileStatement>::MayFail_(MayFail<MayFail_<WhileBlock>> block, MayFail<Expression_> condition, bool until_loop)
        : block(block), condition(condition), until_loop(until_loop){}

MayFail_<DoWhileStatement>::MayFail_(DoWhileStatement doWhileStmt) {
    this->block = wrap(doWhileStmt.block);
    this->condition = wrap_expr(doWhileStmt.condition);
    this->until_loop = doWhileStmt.until_loop;
}

MayFail_<DoWhileStatement>::operator DoWhileStatement() const {
    auto block = unwrap(this->block.value());
    auto condition = unwrap_expr(this->condition.value());
    auto until_loop = this->until_loop;
    return DoWhileStatement{block, condition, until_loop};
}
