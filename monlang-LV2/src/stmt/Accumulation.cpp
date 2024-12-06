#include <monlang-LV2/stmt/Accumulation.h>
#include <monlang-LV2/precedence.h>

/* impl only */

#include <monlang-LV2/expr/Lvalue.h>

#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/ProgramSentence.h>

#include <utils/vec-utils.h>
#include <utils/assert-utils.h>

#define unless(x) if(!(x))

bool peekAccumulation(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 2) {
        return false;
    }

    auto pw = sentence.programWords[1];
    unless (std::holds_alternative<Atom*>(pw)) {
        return false;
    }

    auto atom = *std::get<Atom*>(pw);
    unless (atom.value.back() == '=') {
        return false;
    }
    auto optr = atom.value.substr(0, atom.value.size() - 1);

    auto optr_found = false;
    for (auto [operators, _]: PRECEDENCE_TABLE) {
        if (vec_contains(operators, optr)) {
            optr_found = true;
            break;
        }
    }

    return optr_found;
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


    unless (holds_word(sentence.programWords[0])) {
        return Malformed(MayFail_<Accumulation>{Lvalue(), optr, Expression_()}, ERR(221));
    }
    auto word = get_word(sentence.programWords[0]);
    // NOTE: for the moment `peekLvalue()` only check if word is Atom. In the future will be more descriptive.
    unless (peekLvalue(word)) {
        return Malformed(MayFail_<Accumulation>{Lvalue(), optr, Expression_()}, ERR(222));
    }
    auto variable = buildLvalue(word);


    unless (sentence.programWords.size() >= 3) {
        return Malformed(MayFail_<Accumulation>{variable, optr, Expression_()}, ERR(223));
    }
    auto value_as_term = extractValue(sentence);
    unless (value_as_term) {
        return Malformed(MayFail_<Accumulation>{variable, optr, Expression_()}, ERR(224));
    }
    auto value = buildExpression(*value_as_term);
    if (value.has_error()) {
        return Malformed(MayFail_<Accumulation>{variable, optr, value}, ERR(225));
    }


    return MayFail_<Accumulation>{variable, optr, value};
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

    std::vector<Word> words;
    for (auto e: value_as_sentence) {
        unless (holds_word(e)) {
            return {};
        }
        words.push_back(get_word(e));
    }
    return Term{words};
}

MayFail_<Accumulation>::MayFail_(Lvalue variable, identifier_t operator_, MayFail<Expression_> value)
        : variable(variable), operator_(operator_), value(value){}

MayFail_<Accumulation>::MayFail_(Accumulation accumulation) {
    this->variable = accumulation.variable;
    this->operator_ = accumulation.operator_;
    this->value = wrap_expr(accumulation.value);
}

MayFail_<Accumulation>::operator Accumulation() const {
    auto value = unwrap_expr(this->value.value());
    return Accumulation{this->variable, this->operator_, value};
}

template <>
Accumulation unwrap(const MayFail_<Accumulation>& accumulation) {
    return (Accumulation)accumulation;
}

template <>
MayFail_<Accumulation> wrap(const Accumulation& accumulation) {
    return MayFail_<Accumulation>(accumulation);
}
