#include <monlang-LV2/stmt/Accumulation.h>
#include <monlang-LV2/precedence.h>

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

#include <utils/vec-utils.h>
#include <utils/assert-utils.h>

#define unless(x) if(!(x))

#define SET_TOKEN_FIELDS(accumulation, sentence) \
    accumulation._tokenLeadingNewlines = sentence._tokenLeadingNewlines; \
    accumulation._tokenIndentSpaces = sentence._tokenIndentSpaces; \
    accumulation._tokenLen = sentence._tokenLen; \
    accumulation._tokenTrailingNewlines = sentence._tokenTrailingNewlines

#define SET_MALFORMED_TOKEN_FIELDS(malformed, sentence) \
    malformed.val._tokenLeadingNewlines = sentence._tokenLeadingNewlines; \
    malformed.val._tokenIndentSpaces = sentence._tokenIndentSpaces

const std::string Accumulation::SEPARATOR_SUFFIX = "=";

bool peekAccumulation(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 2) {
        return false;
    }

    auto pw = sentence.programWords[1];
    unless (std::holds_alternative<Atom*>(pw)) {
        return false;
    }

    auto atom = *std::get<Atom*>(pw);
    return atom.value.ends_with(Accumulation::SEPARATOR_SUFFIX);
}

static ProgramSentence consumeSentence(LV1::Program&);

// where 'value' are the [2], [3], .. words from the sentence
// ..returns empty opt if any non-word
static std::optional<Term> extractValue(const ProgramSentence&);

MayFail<MayFail_<Accumulation>> consumeAccumulation(LV1::Program& prog) {
    auto sentence = consumeSentence(prog);
    ASSERT (sentence.programWords.size() >= 2);


    ASSERT (std::holds_alternative<Atom*>(sentence.programWords[1]));
    auto atom = *std::get<Atom*>(sentence.programWords[1]);
    auto optr = atom.value.substr(0, atom.value.size() - 1);
    auto optr_found = false;
    for (auto [operators, _]: PRECEDENCE_TABLE) {
        if (vec_contains(operators, optr)) {
            optr_found = true;
            break;
        }
    }
    unless (optr_found) {
        auto malformed = Malformed(MayFail_<Accumulation>{Lvalue(), std::string(), Expression_()}, ERR(226));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }

    unless (holds_word(sentence.programWords[0])) {
        auto malformed = Malformed(MayFail_<Accumulation>{Lvalue(), optr, Expression_()}, ERR(221));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    auto word = get_word(sentence.programWords[0]);
    // NOTE: for the moment `peekLvalue()` only check if word is Atom. In the future will be more descriptive.
    unless (peekLvalue(word)) {
        auto malformed = Malformed(MayFail_<Accumulation>{Lvalue(), optr, Expression_()}, ERR(222));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    auto variable = buildLvalue(word);


    unless (sentence.programWords.size() >= 3) {
        auto malformed = Malformed(MayFail_<Accumulation>{variable, optr, Expression_()}, ERR(223));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    auto value_as_term = extractValue(sentence);
    unless (value_as_term) {
        auto malformed = Malformed(MayFail_<Accumulation>{variable, optr, Expression_()}, ERR(224));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    auto value = buildExpression(*value_as_term);
    if (value.has_error()) {
        auto malformed = Malformed(MayFail_<Accumulation>{variable, optr, value}, ERR(225));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }

    auto accumulation = MayFail_<Accumulation>{variable, optr, value};
    SET_TOKEN_FIELDS(accumulation, /*from*/ sentence);
    return accumulation;
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

Atom Accumulation::SEPARATOR() {
    auto str = operator_ + Accumulation::SEPARATOR_SUFFIX;
    auto atom = Atom{str};
    atom._tokenLen = str.size();
    return atom;
}

Atom MayFail_<Accumulation>::SEPARATOR() {
    auto str = operator_ + Accumulation::SEPARATOR_SUFFIX;
    auto atom = Atom{str};
    atom._tokenLen = str.size();
    return atom;
}

Accumulation::Accumulation(const Lvalue& variable, const identifier_t& operator_, const Expression& value)
        : variable(variable), operator_(operator_), value(value){}

MayFail_<Accumulation>::MayFail_(Lvalue variable, identifier_t operator_, MayFail<Expression_> value)
        : variable(variable), operator_(operator_), value(value){}

MayFail_<Accumulation>::MayFail_(const Accumulation& accumulation) {
    this->variable = accumulation.variable;
    this->operator_ = accumulation.operator_;
    this->value = wrap_expr(accumulation.value);

    this->_tokenLeadingNewlines = accumulation._tokenLeadingNewlines;
    this->_tokenIndentSpaces = accumulation._tokenIndentSpaces;
    this->_tokenLen = accumulation._tokenLen;
    this->_tokenTrailingNewlines = accumulation._tokenTrailingNewlines;
}

MayFail_<Accumulation>::operator Accumulation() const {
    auto variable = this->variable;
    auto operator_ = this->operator_;
    auto value = unwrap_expr(this->value.value());
    auto accumulation = Accumulation{variable, operator_, value};

    accumulation._tokenLeadingNewlines = this->_tokenLeadingNewlines;
    accumulation._tokenIndentSpaces = this->_tokenIndentSpaces;
    accumulation._tokenLen = this->_tokenLen;
    accumulation._tokenTrailingNewlines = this->_tokenTrailingNewlines;

    return accumulation;
}
