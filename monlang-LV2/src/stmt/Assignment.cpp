#include <monlang-LV2/stmt/Assignment.h>

/* impl only */

#include <monlang-LV2/expr/Lvalue.h>

#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/ProgramSentence.h>
/* require knowing all words for token_len() */
#include <monlang-LV1/ast/ParenthesesGroup.h>
#include <monlang-LV1/ast/SquareBracketsGroup.h>
#include <monlang-LV1/ast/CurlyBracketsGroup.h>
#include <monlang-LV1/ast/PostfixParenthesesGroup.h>
#include <monlang-LV1/ast/PostfixSquareBracketsGroup.h>
#include <monlang-LV1/ast/Association.h>

#include <utils/assert-utils.h>

#define unless(x) if(!(x))

bool peekAssignment(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 2) {
        return false;
    }

    auto pw = sentence.programWords[1];
    unless (std::holds_alternative<Atom*>(pw)) {
        return false;
    }

    auto atom = *std::get<Atom*>(pw);
    return atom.value == ":=";
}

static ProgramSentence consumeSentence(LV1::Program&);

// where 'value' are the [2], [3], .. words from the sentence
// ..returns empty opt if any non-word
static std::optional<Term> extractValue(const ProgramSentence&);

MayFail<MayFail_<Assignment>> consumeAssignment(LV1::Program& prog) {
    auto sentence = consumeSentence(prog);
    ASSERT (sentence.programWords.size() >= 2);


    unless (holds_word(sentence.programWords[0])) {
        return Malformed(MayFail_<Assignment>{Lvalue(), Expression_()}, ERR(211));
    }
    auto word = get_word(sentence.programWords[0]);
    // NOTE: for the moment `peekLvalue()` only check if word is Atom. In the future will be more descriptive.
    unless (peekLvalue(word)) {
        return Malformed(MayFail_<Assignment>{Lvalue(), Expression_()}, ERR(212));
    }
    auto variable = buildLvalue(word);


    unless (sentence.programWords.size() >= 3) {
        return Malformed(MayFail_<Assignment>{variable, Expression_()}, ERR(213));
    }
    auto value_as_term = extractValue(sentence);
    unless (value_as_term) {
        return Malformed(MayFail_<Assignment>{variable, Expression_()}, ERR(214));
    }
    auto value = buildExpression(*value_as_term);
    if (value.has_error()) {
        return Malformed(MayFail_<Assignment>{variable, value}, ERR(215));
    }

    auto assignment = MayFail_<Assignment>{variable, value};
    assignment._tokenLeadingNewlines = sentence._tokenLeadingNewlines;
    assignment._tokenIndentSpaces = sentence._tokenIndentSpaces;
    assignment._tokenLen = sentence._tokenLen;
    assignment._tokenTrailingNewlines = sentence._tokenTrailingNewlines;
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

MayFail_<Assignment>::MayFail_(const Lvalue& variable, const MayFail<Expression_>& value)
        : variable(variable), value(value){}

MayFail_<Assignment>::MayFail_(const Assignment& assignment) {
    this->variable = assignment.variable;
    this->value = wrap_expr(assignment.value);

    this->_tokenLeadingNewlines = assignment._tokenLeadingNewlines;
    this->_tokenIndentSpaces = assignment._tokenIndentSpaces;
    this->_tokenLen = assignment._tokenLen;
    this->_tokenTrailingNewlines = assignment._tokenTrailingNewlines;
}

MayFail_<Assignment>::operator Assignment() const {
    auto variable = this->variable;
    auto value = unwrap_expr(this->value.value());
    auto assignment = Assignment{variable, value};

    assignment._tokenLeadingNewlines = this->_tokenLeadingNewlines;
    assignment._tokenIndentSpaces = this->_tokenIndentSpaces;
    assignment._tokenLen = this->_tokenLen;
    assignment._tokenTrailingNewlines = this->_tokenTrailingNewlines;

    return assignment;
}
