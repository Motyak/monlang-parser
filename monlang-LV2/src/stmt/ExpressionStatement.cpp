#include <monlang-LV2/stmt/ExpressionStatement.h>

#include <utils/assert-utils.h>
#include <utils/variant-utils.h>

static ProgramSentence consumeSentence(LV1::Program&);

MayFail<MayFail_<ExpressionStatement>> consumeExpressionStatement(LV1::Program& prog) {
    auto sentence = consumeSentence(prog);
    ASSERT (sentence.programWords.size() > 0);

    auto term = (Term)sentence;
    auto expression = buildExpression(term);
    if (expression.has_error()) {
        return Malformed(MayFail_<ExpressionStatement>{expression}, ERR(591));
    }
    return MayFail_<ExpressionStatement>{expression};
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
