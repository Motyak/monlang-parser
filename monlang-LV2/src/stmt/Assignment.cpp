#include <monlang-LV2/stmt/Assignment.h>

/* impl only */

#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/ProgramSentence.h>
/* require knowing all words for token_len() */
#include <monlang-LV1/ast/token_len.h>

#include <utils/assert-utils.h>

#define unless(x) if(!(x))

#define SET_TOKEN_FIELDS(assignment, sentence) \
    assignment._tokenLeadingNewlines = sentence._tokenLeadingNewlines; \
    assignment._tokenIndentSpaces = sentence._tokenIndentSpaces; \
    assignment._tokenLen = sentence._tokenLen; \
    assignment._tokenTrailingNewlines = sentence._tokenTrailingNewlines

#define SET_MALFORMED_TOKEN_FIELDS(malformed, sentence) \
    malformed.val._tokenLeadingNewlines = sentence._tokenLeadingNewlines; \
    malformed.val._tokenIndentSpaces = sentence._tokenIndentSpaces

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

const std::string Assignment::SEPARATOR = ":=";

bool peekAssignment(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 2) {
        return false;
    }

    auto pw = sentence.programWords[1];
    unless (std::holds_alternative<Atom*>(pw)) {
        return false;
    }

    auto atom = *std::get<Atom*>(pw);
    return atom.value == Assignment::SEPARATOR;

}

static ProgramSentence consumeSentence(LV1::Program&);

// where 'value' are the [2], [3], .. words from the sentence
// ..returns empty opt if any non-word
static std::optional<Term> extractValue(const ProgramSentence&);

MayFail<MayFail_<Assignment>> consumeAssignment(LV1::Program& prog) {
    auto sentence = consumeSentence(prog);
    ASSERT (sentence.programWords.size() >= 2);


    unless (holds_word(sentence.programWords[0])) {
        auto malformed = Malformed(MayFail_<Assignment>{STUB(Lvalue_), StubExpression_()}, ERR(211));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    auto word = get_word(sentence.programWords[0]);
    auto expr = buildExpression((Term)word);
    if (!is_lvalue(expr.val)) {
        auto malformed = Malformed(MayFail_<Assignment>{STUB(Lvalue_), StubExpression_()}, ERR(212));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }

    auto variable = mayfail_cast<Lvalue_>(expr);
    if (variable.has_error()) {
        auto malformed = Malformed(MayFail_<Assignment>{variable, StubExpression_()}, ERR(216));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }


    unless (sentence.programWords.size() >= 3) {
        auto malformed = Malformed(MayFail_<Assignment>{variable, StubExpression_()}, ERR(213));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    auto value_as_term = extractValue(sentence);
    unless (value_as_term) {
        auto error = ERR(214);
        SET_NON_WORD_ERR_OFFSET(error);
        auto malformed = Malformed(MayFail_<Assignment>{variable, StubExpression_()}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    auto value = buildExpression(*value_as_term);
    if (value.has_error()) {
        auto malformed = Malformed(MayFail_<Assignment>{variable, value}, ERR(215));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }

    auto assignment = MayFail_<Assignment>{variable, value};
    SET_TOKEN_FIELDS(assignment, /*from*/ sentence);
    return assignment;
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

static std::optional<Term> extractValue(const ProgramSentence& sentence) {
    ASSERT (sentence.programWords.size() >= 3);
    auto value_as_sentence = std::vector<ProgramWord>(
        sentence.programWords.begin() + 2,
        sentence.programWords.end()
    );

    size_t wordsTokenLen = 0;
    std::vector<Word> words;
    for (auto e: value_as_sentence) {
        unless (holds_word(e)) {
            return {};
        }
        auto word = get_word(e);
        words.push_back(word);
        wordsTokenLen += token_len(word);
    }

    auto term = Term{words};
    term._tokenLen = wordsTokenLen
            + (words.size() - 1) * sequenceLen(Term::CONTINUATOR_SEQUENCE);
    return term;
}

Assignment::Assignment(const Lvalue& variable, const Expression& value) : variable(variable), value(value){}

MayFail_<Assignment>::MayFail_(const MayFail<Lvalue_>& variable, const MayFail<Expression_>& value)
        : variable(variable), value(value){}

MayFail_<Assignment>::MayFail_(const Assignment& assignment)
        : variable(wrap_lvalue(assignment.variable))
{
    this->value = wrap_expr(assignment.value);

    this->_tokenLeadingNewlines = assignment._tokenLeadingNewlines;
    this->_tokenIndentSpaces = assignment._tokenIndentSpaces;
    this->_tokenLen = assignment._tokenLen;
    this->_tokenTrailingNewlines = assignment._tokenTrailingNewlines;
}

MayFail_<Assignment>::operator Assignment() const {
    auto variable = unwrap_lvalue(this->variable.value());
    auto value = unwrap_expr(this->value.value());
    auto assignment = Assignment{variable, value};

    assignment._tokenLeadingNewlines = this->_tokenLeadingNewlines;
    assignment._tokenIndentSpaces = this->_tokenIndentSpaces;
    assignment._tokenLen = this->_tokenLen;
    assignment._tokenTrailingNewlines = this->_tokenTrailingNewlines;

    return assignment;
}
