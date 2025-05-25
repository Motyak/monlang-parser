#include <monlang-LV2/stmt/Accumulation.h>
#include <monlang-LV2/precedence.h>

/* impl only */

#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/ProgramSentence.h>
/* require knowing all words for token_len() */
#include <monlang-LV1/ast/token_len.h>

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

// sum token len for all words preceding the nth word..
// ..and add it to error offset
#define SET_NTH_WORD_ERR_OFFSET(error, nth) \
    auto err_offset = size_t(0); \
    for (size_t i = 0; i < nth - 1; ++i) { \
        err_offset += token_len(sentence.programWords[i]); \
        err_offset += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE); \
    } \
    error._info["err_offset"] = err_offset


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

const std::string Accumulation::SEPARATOR_SUFFIX = "=";

bool peekAccumulation(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 2) {
        return false;
    }

    auto pw = sentence.programWords[1];
    unless (std::holds_alternative<Atom*>(pw)) {
        return false;
    }

    auto atom_val = std::get<Atom*>(pw)->value;
    for (auto [operators, _]: PRECEDENCE_TABLE) {
        unless (!vec_contains(operators, atom_val)) {
            return false;
        }
    }

    return atom_val.ends_with(Accumulation::SEPARATOR_SUFFIX);
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
    auto optr = Symbol{atom.value.substr(0, atom.value.size() - 1)};
    optr._tokenLen = atom._tokenLen;
    auto optr_found = false;
    for (auto [operators, _]: PRECEDENCE_TABLE) {
        if (vec_contains(operators, optr.name)) {
            optr_found = true;
            break;
        }
    }
    unless (optr_found) {
        auto error = ERR(226);
        SET_NTH_WORD_ERR_OFFSET(error, /*nth*/2);
        auto malformed = Malformed(MayFail_<Accumulation>{STUB(Lvalue_), std::string(), StubExpression_()}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }

    unless (holds_word(sentence.programWords[0])) {
        auto malformed = Malformed(MayFail_<Accumulation>{STUB(Lvalue_), optr, StubExpression_()}, ERR(221));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }

    auto word = get_word(sentence.programWords[0]);
    auto expr = buildExpression((Term)word);
    if (!is_lvalue(expr.val)) {
        auto malformed = Malformed(MayFail_<Accumulation>{STUB(Lvalue_), optr, StubExpression_()}, ERR(222));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }

    auto variable = mayfail_cast<Lvalue_>(expr);
    if (variable.has_error()) {
        auto malformed = Malformed(MayFail_<Accumulation>{variable, optr, StubExpression_()}, ERR(227));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }

    unless (sentence.programWords.size() >= 3) {
        auto malformed = Malformed(MayFail_<Accumulation>{variable, optr, StubExpression_()}, ERR(223));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }

    auto value_as_term = extractValue(sentence);
    unless (value_as_term) {
        auto error = ERR(224);
        SET_NON_WORD_ERR_OFFSET(error);
        auto malformed = Malformed(MayFail_<Accumulation>{variable, optr, StubExpression_()}, error);
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

std::string Accumulation::SEPARATOR() {
    return operator_.name + Accumulation::SEPARATOR_SUFFIX;
}

std::string MayFail_<Accumulation>::SEPARATOR() {
    return operator_.name + Accumulation::SEPARATOR_SUFFIX;
}

Accumulation::Accumulation(const Lvalue& variable, const Symbol& operator_, const Expression& value)
        : variable(variable), operator_(operator_), value(value){}

MayFail_<Accumulation>::MayFail_(const MayFail<Lvalue_>& variable, const Symbol& operator_, const MayFail<Expression_>& value)
        : variable(variable), operator_(operator_), value(value){}

MayFail_<Accumulation>::MayFail_(const Accumulation& accumulation)
        : variable(wrap_lvalue(accumulation.variable))
{
    this->operator_ = accumulation.operator_;
    this->value = wrap_expr(accumulation.value);

    this->_tokenLeadingNewlines = accumulation._tokenLeadingNewlines;
    this->_tokenIndentSpaces = accumulation._tokenIndentSpaces;
    this->_tokenLen = accumulation._tokenLen;
    this->_tokenTrailingNewlines = accumulation._tokenTrailingNewlines;
    this->_tokenId = accumulation._tokenId;
}

MayFail_<Accumulation>::operator Accumulation() const {
    auto variable = unwrap_lvalue(this->variable.value());
    auto operator_ = this->operator_;
    auto value = unwrap_expr(this->value.value());
    auto accumulation = Accumulation{variable, operator_, value};

    accumulation._tokenLeadingNewlines = this->_tokenLeadingNewlines;
    accumulation._tokenIndentSpaces = this->_tokenIndentSpaces;
    accumulation._tokenLen = this->_tokenLen;
    accumulation._tokenTrailingNewlines = this->_tokenTrailingNewlines;
    accumulation._tokenId = this->_tokenId;

    return accumulation;
}
