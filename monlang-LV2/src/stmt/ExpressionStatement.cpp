#include <monlang-LV2/stmt/ExpressionStatement.h>

#include <utils/assert-utils.h>
#include <utils/variant-utils.h>

#define SET_TOKEN_FIELDS(assignment, sentence) \
    assignment._tokenLeadingNewlines = sentence._tokenLeadingNewlines; \
    assignment._tokenIndentSpaces = sentence._tokenIndentSpaces; \
    assignment._tokenLen = sentence._tokenLen; \
    assignment._tokenTrailingNewlines = sentence._tokenTrailingNewlines

#define SET_MALFORMED_TOKEN_FIELDS(malformed, sentence) \
    malformed.val._tokenLeadingNewlines = sentence._tokenLeadingNewlines; \
    malformed.val._tokenIndentSpaces = sentence._tokenIndentSpaces; \
    malformed.val._tokenLen = sentence._tokenLen; \
    malformed.val._tokenTrailingNewlines = sentence._tokenTrailingNewlines

static ProgramSentence consumeSentence(LV1::Program&);

MayFail<MayFail_<ExpressionStatement>> consumeExpressionStatement(LV1::Program& prog) {
    auto sentence = consumeSentence(prog);
    ASSERT (sentence.programWords.size() > 0);

    // TODO: if sentence contains any ProgramWord => return malformed expr stmt accordingly
    auto term = (Term)sentence;
    auto expression = buildExpression(term);
    if (expression.has_error()) {
        auto malformed = Malformed(MayFail_<ExpressionStatement>{expression}, ERR(591));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }

    auto expressionStatement = MayFail_<ExpressionStatement>{expression};
    SET_TOKEN_FIELDS(expressionStatement, /*from*/ sentence);
    return expressionStatement;
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

ExpressionStatement::ExpressionStatement(const Expression& expression)
        : expression(expression){}

MayFail_<ExpressionStatement>::MayFail_(MayFail<Expression_> expression) : expression(expression){}

MayFail_<ExpressionStatement>::MayFail_(ExpressionStatement exprStmt) {
    this->expression = wrap_expr(exprStmt.expression);

    this->_tokenLeadingNewlines = exprStmt._tokenLeadingNewlines;
    this->_tokenIndentSpaces = exprStmt._tokenIndentSpaces;
    this->_tokenLen = exprStmt._tokenLen;
    this->_tokenTrailingNewlines = exprStmt._tokenTrailingNewlines;
}

MayFail_<ExpressionStatement>::operator ExpressionStatement() const {
    auto expression = unwrap_expr(this->expression.value());
    auto exprStmt = ExpressionStatement{expression};

    exprStmt._tokenLeadingNewlines = this->_tokenLeadingNewlines;
    exprStmt._tokenIndentSpaces = this->_tokenIndentSpaces;
    exprStmt._tokenLen = this->_tokenLen;
    exprStmt._tokenTrailingNewlines = this->_tokenTrailingNewlines;

    return exprStmt;
}
