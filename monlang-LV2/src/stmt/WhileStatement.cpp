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
    auto iterate_at_least_once = false;
    auto sentence = consumeSentence(prog);


    /* while/until keyword */
    ASSERT (std::holds_alternative<Atom*>(sentence.programWords[0]));
    auto atom = *std::get<Atom*>(sentence.programWords[0]);
    auto until_loop = atom.value == "until";


    /* while condition */
    unless (sentence.programWords.size() >= 2) {
        return Malformed(MayFail_<WhileStatement>{Expression_(), MayFail_<BlockExpression>(), false, iterate_at_least_once}, ERR(331));
    }
    auto pw = sentence.programWords[1];
    unless (std::holds_alternative<SquareBracketsTerm*>(pw)) {
        return Malformed(MayFail_<WhileStatement>{Expression_(), MayFail_<BlockExpression>(), false, iterate_at_least_once}, ERR(332));
    }
    auto sbt = *std::get<SquareBracketsTerm*>(pw);
    auto condition = buildExpression(sbt.term);
    if (condition.has_error()) {
        return Malformed(MayFail_<WhileStatement>{condition, MayFail_<BlockExpression>(), false, iterate_at_least_once}, ERR(333));
    }


    /* while loop */
    unless (sentence.programWords.size() >= 3) {
        return Malformed(MayFail_<WhileStatement>{condition, MayFail_<BlockExpression>(), false, iterate_at_least_once}, ERR(334));
    }
    auto block_as_pw = sentence.programWords[2];
    unless (holds_word(block_as_pw)) {
        return Malformed(MayFail_<WhileStatement>{condition, MayFail_<BlockExpression>(), false, iterate_at_least_once}, ERR(335));
    }
    auto block_as_word = get_word(block_as_pw);
    auto block = buildBlockExpression(block_as_word);
    if (block.has_error()) {
        return Malformed(MayFail_<WhileStatement>{condition, block, false, iterate_at_least_once}, ERR(336));
    }


    /* check if additional words */
    if (sentence.programWords.size() > 3) {
        return Malformed(MayFail_<WhileStatement>{condition, block, false, iterate_at_least_once}, ERR(337));
    }


    return MayFail_<WhileStatement>{condition, block, until_loop, iterate_at_least_once};
}

MayFail<MayFail_<WhileStatement>> consumeDoWhileStatement(LV1::Program& prog) {
    auto iterate_at_least_once = true;
    auto currSentence = consumeSentence(prog);

    /* while loop */
    unless (currSentence.programWords.size() >= 2) {
        return Malformed(MayFail_<WhileStatement>{Expression_(), MayFail_<BlockExpression>(), false, iterate_at_least_once}, ERR(341));
    }
    auto block_as_pw = currSentence.programWords[1];
    unless (holds_word(block_as_pw)) {
        return Malformed(MayFail_<WhileStatement>{Expression_(), MayFail_<BlockExpression>(), false, iterate_at_least_once}, ERR(342));
    }
    auto block_as_word = get_word(block_as_pw);
    auto block = buildBlockExpression(block_as_word);
    if (block.has_error()) {
        return Malformed(MayFail_<WhileStatement>{Expression_(), block, false, iterate_at_least_once}, ERR(343));
    }


    /* check if additional words */
    if (currSentence.programWords.size() > 2) {
        return Malformed(MayFail_<WhileStatement>{Expression_(), block, false, iterate_at_least_once}, ERR(344));
    }


    currSentence = consumeSentence(prog);

    /* while/until keyword */
    ASSERT (std::holds_alternative<Atom*>(currSentence.programWords[0]));
    auto atom = *std::get<Atom*>(currSentence.programWords[0]);
    auto until_loop = atom.value == "until";


    /* while condition */
    unless (currSentence.programWords.size() >= 2) {
        return Malformed(MayFail_<WhileStatement>{Expression_(), block, until_loop, iterate_at_least_once}, ERR(345));
    }
    auto pw = currSentence.programWords[1];
    unless (std::holds_alternative<SquareBracketsTerm*>(pw)) {
        return Malformed(MayFail_<WhileStatement>{Expression_(), block, until_loop, iterate_at_least_once}, ERR(346));
    }
    auto sbt = *std::get<SquareBracketsTerm*>(pw);
    auto condition = buildExpression(sbt.term);
    if (condition.has_error()) {
        return Malformed(MayFail_<WhileStatement>{condition, block, until_loop, iterate_at_least_once}, ERR(347));
    }


    /* check if additional words */
    if (currSentence.programWords.size() > 2) {
        return Malformed(MayFail_<WhileStatement>{condition, block, until_loop, iterate_at_least_once}, ERR(348));
    }


    return MayFail_<WhileStatement>{condition, block, until_loop, iterate_at_least_once};
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
