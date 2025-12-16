#include <monlang-LV2/expr/MapLiteral.h>

/* impl only */

#include <monlang-LV2/expr/Lambda.h>

#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/SquareBracketsGroup.h>
#include <monlang-LV1/ast/MultilineSquareBracketsGroup.h>
#include <monlang-LV1/ast/Association.h>
/* require knowing all words for token_len() */
#include <monlang-LV1/ast/token_len.h>

#include <utils/assert-utils.h>
#include <utils/variant-utils.h>
#include <utils/loop-utils.h>

#define unless(x) if(!(x))

// sum token len for all arguments preceding the nth argument..
// ..and add it to error offset
#define SET_NTH_ARG_ERR_OFFSET(error, nth) \
    auto err_offset = size_t(0); \
    err_offset += sequenceLen(SquareBracketsGroup::INITIATOR_SEQUENCE); \
    for (size_t i = 0; i < nth - 1; ++i) { \
        err_offset += token_len(sbg.terms[i]); \
        err_offset += sequenceLen(SquareBracketsGroup::CONTINUATOR_SEQUENCE); \
    } \
    error._info["err_offset"] = err_offset

#define SET_NTH_SENTENCE_ERR_OFFSET(error, nth) \
    auto err_offset = size_t(0); \
    err_offset += sequenceLen(SquareBracketsGroup::INITIATOR_SEQUENCE); \
    err_offset += 1; /*newline*/ \
    size_t i = 0; \
    for (; i < __nth_it - 1; ++i) { \
        err_offset += msbg.sentences.at(i)._tokenLeadingNewlines; \
        err_offset += msbg.sentences.at(i)._tokenIndentSpaces; \
        err_offset += msbg.sentences.at(i)._tokenLen; \
    } \
    err_offset += msbg.sentences.at(i)._tokenLeadingNewlines; \
    err_offset += msbg.sentences.at(i)._tokenIndentSpaces; \
    error._info["err_offset"] = err_offset

// nth sentence, 3rd word
#define SET_NTH_SENTENCE_VAL_ERR_OFFSET(error, nth) \
    SET_NTH_SENTENCE_ERR_OFFSET(error, nth); \
    err_offset += token_len(sentence.programWords.at(0)); \
    err_offset += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE); \
    err_offset += 2; /*=>*/ \
    err_offset += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE); \
    error._info["err_offset"] = err_offset

bool peekMapLiteral(const Word& word) {

    if (std::holds_alternative<Atom*>(word) && std::get<Atom*>(word)->value == "[:]") {
        return true; // special case
    }

    if (std::holds_alternative<SquareBracketsGroup*>(word)) {
        auto sbg = *std::get<SquareBracketsGroup*>(word);
        unless (!sbg.terms.empty()) return false;

        auto term = sbg.terms.at(0);
        ASSERT (term.words.size() > 0);
        unless (term.words.size() == 1) return false;

        auto term_word = term.words.at(0);
        if (peekLambda(term_word)) {
            return false;
        }

        return std::holds_alternative<Association*>(term_word);
    }

    else if (std::holds_alternative<MultilineSquareBracketsGroup*>(word)) {
        auto msbg = *std::get<MultilineSquareBracketsGroup*>(word);
        unless (!msbg.sentences.empty()) return false;

        auto sentence = msbg.sentences.at(0);
        unless (sentence.programWords.size() >= 3) return false;

        unless (std::holds_alternative<Atom*>(sentence.programWords.at(1))) return false;
        auto atom = std::get<Atom*>(sentence.programWords.at(1));
        return atom->value == "=>";
    }

    else {
        return false;
    }
}

static MayFail<MayFail_<MapLiteral>> buildOnelineMapLiteral(const SquareBracketsGroup&);
static MayFail<MayFail_<MapLiteral>> buildMultilineMapLiteral(const MultilineSquareBracketsGroup&);

MayFail<MayFail_<MapLiteral>> buildMapLiteral(const Word& word) {
    return std::visit(overload{
        [](Atom* atom) -> MayFail<MayFail_<MapLiteral>> {
            ASSERT (atom->value == "[:]");
            auto emptyMapLiteral = MayFail_<MapLiteral>{};
            emptyMapLiteral._tokenLen = 3; // [:]
            return emptyMapLiteral;
        },
        [](SquareBracketsGroup* sbg) -> MayFail<MayFail_<MapLiteral>> {
            return buildOnelineMapLiteral(*sbg);
        },
        [](MultilineSquareBracketsGroup* msbg) -> MayFail<MayFail_<MapLiteral>> {
            return buildMultilineMapLiteral(*msbg);
        },
        [](auto*) -> MayFail<MayFail_<MapLiteral>> {
            SHOULD_NOT_HAPPEN();
        },
    }, word);
}

static MayFail<MayFail_<MapLiteral>> buildOnelineMapLiteral(const SquareBracketsGroup& sbg) {
    auto arguments = std::vector<MayFail_<MapLiteral>::Argument>();

    LOOP for (auto term: sbg.terms) {
        if (__first_it) {
            ASSERT (term.words.size() == 1);
        }
        else unless (term.words.size() == 1) {
            auto error = ERR(691_0);
            SET_NTH_ARG_ERR_OFFSET(error, __nth_it);
            return Malformed(MayFail_<MapLiteral>{arguments}, error);
        }

        auto word = term.words.at(0);
        if (__first_it) {
            ASSERT (std::holds_alternative<Association*>(word));
        }
        else unless (std::holds_alternative<Association*>(word)) {
            auto error = ERR(691);
            SET_NTH_ARG_ERR_OFFSET(error, __nth_it);
            return Malformed(MayFail_<MapLiteral>{arguments}, error);
        }

        auto assoc = *std::get<Association*>(word);

        auto key = buildExpression((Term)variant_cast(assoc.leftPart));
        if (key.has_error()) {
            arguments.push_back({key, StubExpression_()});
            return Malformed(MayFail_<MapLiteral>{arguments}, ERR(692));
        }

        auto val = buildExpression((Term)assoc.rightPart);
        if (val.has_error()) {
            arguments.push_back({key, val});
            return Malformed(MayFail_<MapLiteral>{arguments}, ERR(693));
        }

        arguments.push_back({key, val});

        ENDLOOP
    }

    auto mapLiteral = MayFail_<MapLiteral>{arguments};
    mapLiteral._tokenLen = sbg._tokenLen;
    return mapLiteral;
}

static std::optional<Term> extractValue(const ProgramSentence&);

static MayFail<MayFail_<MapLiteral>> buildMultilineMapLiteral(const MultilineSquareBracketsGroup& msbg) {
    auto arguments = std::vector<MayFail_<MapLiteral>::Argument>();

    LOOP for (auto sentence: msbg.sentences) {
        if (__first_it) {
            ASSERT (sentence.programWords.size() >= 3);
            ASSERT (std::holds_alternative<Atom*>(sentence.programWords.at(1)));
            ASSERT (std::get<Atom*>(sentence.programWords.at(1))->value == "=>");
        }
        else {
            unless (sentence.programWords.size() >= 3) {
                auto error = ERR(694);
                SET_NTH_SENTENCE_ERR_OFFSET(error, __nth_it);
                auto malformed = Malformed(MayFail_<MapLiteral>{arguments}, error);
                malformed.val._msbg = msbg;
                return malformed;
            }
            unless (std::holds_alternative<Atom*>(sentence.programWords.at(1))) {
                auto error = ERR(694_0);
                SET_NTH_SENTENCE_ERR_OFFSET(error, __nth_it);
                auto malformed = Malformed(MayFail_<MapLiteral>{arguments}, error);
                malformed.val._msbg = msbg;
                return malformed;
            }
            unless (std::get<Atom*>(sentence.programWords.at(1))->value == "=>") {
                auto error = ERR(694_00);
                SET_NTH_SENTENCE_ERR_OFFSET(error, __nth_it);
                auto malformed = Malformed(MayFail_<MapLiteral>{arguments}, error);
                malformed.val._msbg = msbg;
                return malformed;
            }
        }

        unless (holds_word(sentence.programWords.at(0))) {
            auto error = ERR(695);
            SET_NTH_SENTENCE_ERR_OFFSET(error, __nth_it);
            auto malformed = Malformed(MayFail_<MapLiteral>{arguments}, error);
            malformed.val._msbg = msbg;
            return malformed;
        }
        auto keyWord = get_word(sentence.programWords.at(0));
        auto keyExpr = buildExpression((Term)keyWord);
        if (keyExpr.has_error()) {
            arguments.push_back({keyExpr, StubExpression_()});
            auto error = ERR(696);
            auto malformed = Malformed(MayFail_<MapLiteral>{arguments}, error);
            malformed.val._msbg = msbg;
            return malformed;
        }

        auto valTerm = extractValue(sentence);
        unless (valTerm) {
            auto error = ERR(697);
            SET_NTH_SENTENCE_VAL_ERR_OFFSET(error, __nth_it);
            auto malformed = Malformed(MayFail_<MapLiteral>{arguments}, error);
            malformed.val._msbg = msbg;
            return malformed;
        }
        auto valExpr = buildExpression(*valTerm);
        if (valExpr.has_error()) {
            arguments.push_back({keyExpr, valExpr});
            auto error = ERR(698);
            SET_NTH_SENTENCE_VAL_ERR_OFFSET(error, __nth_it);
            auto malformed = Malformed(MayFail_<MapLiteral>{arguments}, error);
            malformed.val._msbg = msbg;
            return malformed;
        }

        arguments.push_back({keyExpr, valExpr});
        ENDLOOP
    }

    auto mapLiteral = MayFail_<MapLiteral>{arguments};
    mapLiteral._msbg = msbg;
    mapLiteral._tokenLen = msbg._tokenLen;
    return mapLiteral;
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

MapLiteral::MapLiteral(const std::vector<Argument>& arguments)
        : arguments(arguments){}

MayFail_<MapLiteral>::MayFail_(const std::vector<Argument>& arguments)
        : arguments(arguments){}

MayFail_<MapLiteral>::MayFail_(MapLiteral mapLiteral) {
    auto arguments = std::vector<Argument>();
    for (auto [key, val]: mapLiteral.arguments) {
        arguments.push_back({wrap_expr(key), wrap_expr(val)});
    }
    this->arguments = arguments;
    this->_msbg = mapLiteral._msbg;
    this->_tokenLen = mapLiteral._tokenLen;
    this->_tokenId = mapLiteral._tokenId;
}

MayFail_<MapLiteral>::operator MapLiteral() const {
    auto arguments = std::vector<MapLiteral::Argument>();
    for (auto [key, val]: this->arguments) {
        arguments.push_back({unwrap_expr(key.value()), unwrap_expr(val.value())});
    }
    auto mapLiteral = MapLiteral{arguments};
    mapLiteral._msbg = this->_msbg;
    mapLiteral._tokenLen = this->_tokenLen;
    mapLiteral._tokenId = this->_tokenId;
    return mapLiteral;
}
