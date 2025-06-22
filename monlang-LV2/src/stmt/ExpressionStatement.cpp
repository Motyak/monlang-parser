#include <monlang-LV2/stmt/ExpressionStatement.h>

#include <utils/assert-utils.h>
#include <utils/variant-utils.h>

#define SET_TOKEN_FIELDS(exprStmt, sentence) \
    exprStmt._tokenLeadingNewlines = sentence._tokenLeadingNewlines; \
    exprStmt._tokenIndentSpaces = sentence._tokenIndentSpaces; \
    exprStmt._tokenLen = sentence._tokenLen; \
    exprStmt._tokenTrailingNewlines = sentence._tokenTrailingNewlines

#define SET_MALFORMED_TOKEN_FIELDS(malformed, sentence) \
    malformed.val._tokenLeadingNewlines = sentence._tokenLeadingNewlines; \
    malformed.val._tokenIndentSpaces = sentence._tokenIndentSpaces

static ProgramSentence consumeSentence(LV1::Program&);

MayFail<MayFail_<ExpressionStatement>> consumeExpressionStatement(LV1::Program& prog) {
    auto sentence = consumeSentence(prog);
    ASSERT (sentence.programWords.size() > 0);

    auto term = (Term)sentence;
    term._tokenLen -= 1;
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
    this->_tokenId = exprStmt._tokenId;
}

MayFail_<ExpressionStatement>::operator ExpressionStatement() const {
    auto expression = unwrap_expr(this->expression.value());
    auto exprStmt = ExpressionStatement{expression};

    exprStmt._tokenLeadingNewlines = this->_tokenLeadingNewlines;
    exprStmt._tokenIndentSpaces = this->_tokenIndentSpaces;
    exprStmt._tokenLen = this->_tokenLen;
    exprStmt._tokenTrailingNewlines = this->_tokenTrailingNewlines;
    exprStmt._tokenId = this->_tokenId;

    return exprStmt;
}
