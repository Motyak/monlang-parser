#include <monlang-LV2/stmt/ExpressionStatement.h>

#include <utils/assert-utils.h>
#include <utils/variant-utils.h>


static Term toTerm(const ProgramSentence&);
static ProgramSentence consumeSentence(LV1::Program&);

MayFail<MayFail_<ExpressionStatement>> consumeExpressionStatement(LV1::Program& prog) {
    auto sentence = consumeSentence(prog);
    ASSERT (sentence.programWords.size() > 0);

    auto term = toTerm(sentence);
    return MayFail_<ExpressionStatement>{buildExpression(term)};
}

static Term toTerm(const ProgramSentence& sentence) {
    std::vector<Word> words;
    for (auto e: sentence.programWords) {
        words.push_back(get_word(e));
    }
    return Term{words};
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

MayFail_<ExpressionStatement>::MayFail_(MayFail<Expression_> expression) : expression(expression){}

MayFail_<ExpressionStatement>::MayFail_(ExpressionStatement exprStmt) {
    this->expression = wrap_expr(exprStmt.expression);
}

MayFail_<ExpressionStatement>::operator ExpressionStatement() const {
    auto expression = unwrap_expr(this->expression.value());
    return ExpressionStatement{expression};
}
