#include <monlang-LV2/stmt/ForeachStatement.h>

/* impl only */

#include <monlang-LV1/ast/Atom.h>

#include <utils/assert-utils.h>

#define unless(x) if(!(x))

#define MALFORMED_STMT(err_msg) \
    malformed_stmt = err_msg; \
    return ForeachStatement()

bool peekForeachStatement(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 1) {
        return false;
    }
    unless (std::holds_alternative<Atom*>(sentence.programWords[0])) {
        return false;
    }
    auto atom = *std::get<Atom*>(sentence.programWords[0]);
    return atom.value == "foreach";
}

static ProgramSentence consumeSentence(LV1::Program&);

// returns empty opt if non-word
static std::optional<Term> extractIterable(const ProgramSentence&);

MayFail<MayFail_<ForeachStatement>> consumeForeachStatement(LV1::Program& prog) {
    auto sentence = consumeSentence(prog);
    unless (sentence.programWords.size() >= 3) {
        return Malformed(MayFail_<ForeachStatement>{Expression_(), MayFail_<BlockExpression>()}, ERR(321));
    }


    auto iterable_as_term = extractIterable(sentence);
    unless (iterable_as_term) {
        return Malformed(MayFail_<ForeachStatement>{Expression_(), MayFail_<BlockExpression>()}, ERR(322));
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
    auto block = buildBlockExpression(word);
    if (block.has_error()) {
        return Malformed(MayFail_<ForeachStatement>{iterable, block}, ERR(325));
    }


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

    std::vector<Word> words;
    for (auto e: iterable_as_sentence) {
        unless (holds_word(e)) {
            return {};
        }
        words.push_back(get_word(e));
    }
    return Term{words};
}

MayFail_<ForeachStatement>::MayFail_(MayFail<Expression_> iterable, MayFail<MayFail_<ForeachBlock>> block)
        : iterable(iterable), block(block){}

MayFail_<ForeachStatement>::MayFail_(ForeachStatement foreachStmt) {
    this->iterable = wrap_expr(foreachStmt.iterable);
    this->block = wrap(foreachStmt.block);
}

MayFail_<ForeachStatement>::operator ForeachStatement() const {
    auto iterable = unwrap_expr(this->iterable.value());
    auto block = unwrap(this->block.value());
    return ForeachStatement{iterable, block};
}
