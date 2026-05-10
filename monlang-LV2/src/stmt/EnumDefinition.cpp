#include <monlang-LV2/stmt/EnumDefinition.h>

/* impl only */

#include <monlang-LV2/Expression.h>

#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/ProgramSentence.h>
/* require knowing all words for token_len() */
#include <monlang-LV1/ast/token_len.h>

#include <utils/assert-utils.h>

#define unless(x) if(!(x))

#define SET_TOKEN_FIELDS(enumDef, sentence) \
    enumDef._tokenLeadingNewlines = sentence._tokenLeadingNewlines; \
    enumDef._tokenIndentSpaces = sentence._tokenIndentSpaces; \
    enumDef._tokenLen = sentence._tokenLen; \
    enumDef._tokenTrailingNewlines = sentence._tokenTrailingNewlines

#define SET_MALFORMED_TOKEN_FIELDS(malformed, sentence) \
    malformed.val._tokenLeadingNewlines = sentence._tokenLeadingNewlines; \
    malformed.val._tokenIndentSpaces = sentence._tokenIndentSpaces

#define SET_MALFORMED_ENUM_VALUE_TOKEN_FIELDS(malformedEnumVal, sentence_) \
    malformedEnumVal.val._tokenLeadingNewlines = sentence_._tokenLeadingNewlines; \
    malformedEnumVal.val._tokenIndentSpaces = sentence_._tokenIndentSpaces; \
    malformedEnumVal.val._tokenTrailingNewlines = sentence_._tokenTrailingNewlines

// sum token len for all words preceding the nth word..
// ..and add it to error offset
#define SET_NTH_WORD_ERR_OFFSET(error, sentence, nth) \
    auto err_offset = size_t(0); \
    for (size_t i = 0; i < nth - 1; ++i) { \
        err_offset += token_len(sentence.programWords[i]); \
        err_offset += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE); \
    } \
    error._info["err_offset"] = err_offset

const std::string EnumDefinition::KEYWORD = "enum";

bool peekEnumDefinition(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 1) {
        return false;
    }

    auto pw = sentence.programWords[0];
    unless (std::holds_alternative<Atom*>(pw)) {
        return false;
    }

    auto atom = *std::get<Atom*>(pw);
    return atom.value == EnumDefinition::KEYWORD;
}

static ProgramSentence consumeSentence(LV1::Program&);

// where 'value' are the [2], [3], .. words from the sentence
static Term extractValue(const ProgramSentence& sentence);

MayFail<MayFail_<EnumDefinition>> consumeEnumDefinition(LV1::Program& prog) {
    auto sentence = consumeSentence(prog);


    unless (sentence.programWords.size() >= 2) {
        auto malformed = Malformed(MayFail_<EnumDefinition>{Symbol(), {}}, ERR(441));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    ASSERT (holds_word(sentence.programWords[1]));
    auto word = get_word(sentence.programWords[1]);
    auto is_an_atom = std::holds_alternative<Atom*>(word);
    auto expr = buildExpression((Term)word);
    unless (std::holds_alternative<Symbol*>(expr.val)) {
        auto error = ERR(442);
        SET_NTH_WORD_ERR_OFFSET(error, sentence, /*nth*/2);
        auto malformed = Malformed(MayFail_<EnumDefinition>{Symbol(), {}}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    auto enum_ = *std::get<Symbol*>(expr.val);

    unless (is_an_atom) {
        auto error = ERR(443);
        SET_NTH_WORD_ERR_OFFSET(error, sentence, /*nth*/2);
        auto malformed = Malformed(MayFail_<EnumDefinition>{Symbol(), {}}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }


    unless (sentence.programWords.size() >= 3) {
        auto malformed = Malformed(MayFail_<EnumDefinition>{enum_, {}}, ERR(444));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }

    auto block_as_pw = sentence.programWords[2];
    ASSERT (holds_word(block_as_pw));
    auto blockAsWord = get_word(block_as_pw);

    unless (std::holds_alternative<CurlyBracketsGroup*>(blockAsWord)) {
        auto error = ERR(445);
        SET_NTH_WORD_ERR_OFFSET(error, sentence, 3);
        auto malformed = Malformed(MayFail_<EnumDefinition>{enum_, {}}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    auto block = *std::get<CurlyBracketsGroup*>(blockAsWord);

    if (block.term) {
        auto error = ERR(446);
        SET_NTH_WORD_ERR_OFFSET(error, sentence, 3);
        auto malformed = Malformed(MayFail_<EnumDefinition>{enum_, {}}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }

    std::vector<MayFail<MayFail_<EnumDefinition>::EnumValue>> enumValues;
    for (auto sentence_: block.sentences) {
        Symbol enumerator;
        MayFail<Expression_> enumerate;

        /* enumerator */
        {
            ASSERT (sentence_.programWords.size() >= 1);
            ASSERT (holds_word(sentence_.programWords[0]));
            auto word = get_word(sentence_.programWords[0]);
            auto is_an_atom = std::holds_alternative<Atom*>(word);
            auto expr = buildExpression((Term)word);
            unless (std::holds_alternative<Symbol*>(expr.val)) {
                auto malformedEnumVal = Malformed(MayFail_<EnumDefinition>::EnumValue{Symbol{}, StubExpression_()}, ERR(451));
                SET_MALFORMED_ENUM_VALUE_TOKEN_FIELDS(malformedEnumVal, sentence_);
                enumValues.push_back(malformedEnumVal);
                auto malformed = Malformed(MayFail_<EnumDefinition>{enum_, enumValues}, ERR(447));
                SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
                return malformed;
            }
            unless (is_an_atom) {
                auto malformedEnumVal = Malformed(MayFail_<EnumDefinition>::EnumValue{Symbol{}, StubExpression_()}, ERR(452));
                SET_MALFORMED_ENUM_VALUE_TOKEN_FIELDS(malformedEnumVal, sentence_);
                enumValues.push_back(malformedEnumVal);
                auto malformed = Malformed(MayFail_<EnumDefinition>{enum_, enumValues}, ERR(447));
                SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
                return malformed;
            }
            enumerator = *std::get<Symbol*>(expr.val);
        }

        unless (sentence_.programWords.size() >= 2) {
            auto malformedEnumVal = Malformed(MayFail_<EnumDefinition>::EnumValue{enumerator, StubExpression_()}, ERR(453));
            SET_MALFORMED_ENUM_VALUE_TOKEN_FIELDS(malformedEnumVal, sentence_);
            enumValues.push_back(malformedEnumVal);
            auto malformed = Malformed(MayFail_<EnumDefinition>{enum_, enumValues}, ERR(447));
            SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
            return malformed;
        }

        auto pw1 = sentence_.programWords[1];
        unless (std::holds_alternative<Atom*>(pw1) && std::get<Atom*>(pw1)->value == "=") {
            auto error = ERR(454);
            SET_NTH_WORD_ERR_OFFSET(error, sentence_, 2);
            auto malformedEnumVal = Malformed(MayFail_<EnumDefinition>::EnumValue{enumerator, StubExpression_()}, error);
            SET_MALFORMED_ENUM_VALUE_TOKEN_FIELDS(malformedEnumVal, sentence_);
            enumValues.push_back(malformedEnumVal);
            auto malformed = Malformed(MayFail_<EnumDefinition>{enum_, enumValues}, ERR(447));
            SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
            return malformed;
        }

        unless (sentence_.programWords.size() >= 3) {
            auto malformedEnumVal = Malformed(MayFail_<EnumDefinition>::EnumValue{enumerator, StubExpression_()}, ERR(455));
            SET_MALFORMED_ENUM_VALUE_TOKEN_FIELDS(malformedEnumVal, sentence_);
            enumValues.push_back(malformedEnumVal);
            auto malformed = Malformed(MayFail_<EnumDefinition>{enum_, enumValues}, ERR(447));
            SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
            return malformed;
        }

        /* enumerate */
        {
            auto term = extractValue(sentence_);
            enumerate = buildExpression(term);
            if (enumerate.has_error()) {
                auto error = ERR(456);
                SET_NTH_WORD_ERR_OFFSET(error, sentence_, 3);
                auto malformedEnumVal = Malformed(MayFail_<EnumDefinition>::EnumValue{enumerator, enumerate}, error);
                SET_MALFORMED_ENUM_VALUE_TOKEN_FIELDS(malformedEnumVal, sentence_);
                enumValues.push_back(malformedEnumVal);
                auto malformed = Malformed(MayFail_<EnumDefinition>{enum_, enumValues}, ERR(447));
                SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
                return malformed;
            }
        }

        auto enumValue = MayFail_<EnumDefinition>::EnumValue{enumerator, enumerate};
        SET_TOKEN_FIELDS(enumValue, sentence_);
        enumValues.push_back(enumValue);
    }

    auto structDef = MayFail_<EnumDefinition>{enum_, enumValues};
    SET_TOKEN_FIELDS(structDef, /*from*/ sentence);
    return structDef;
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

static Term extractValue(const ProgramSentence& sentence) {
    ASSERT (sentence.programWords.size() >= 3);
    auto rhs_as_sentence = std::vector<ProgramWord>(
        sentence.programWords.begin() + 2,
        sentence.programWords.end()
    );

    size_t wordsTokenLen = 0;
    std::vector<Word> words;
    for (auto e: rhs_as_sentence) {
        ASSERT (holds_word(e));
        auto word = get_word(e);
        words.push_back(get_word(e));
        wordsTokenLen += token_len(word);
    }

    auto term = Term{words};
    term._tokenLen = wordsTokenLen
            + (words.size() - 1) * sequenceLen(Term::CONTINUATOR_SEQUENCE);
    return term;
}

EnumDefinition::EnumValue::EnumValue(const Symbol& enumerator, const Expression& enumerate)
        : enumerator(enumerator), enumerate(enumerate){}

EnumDefinition::EnumDefinition(const Symbol& enum_, const std::vector<EnumValue>& enumValues)
        : enum_(enum_), enumValues(enumValues){}

MayFail_<EnumDefinition>::EnumValue::EnumValue(const Symbol& enumerator, const MayFail<Expression_>& enumerate)
        : enumerator(enumerator), enumerate(enumerate){}

MayFail_<EnumDefinition>::MayFail_(const Symbol& enum_, const std::vector<MayFail<MayFail_<EnumDefinition>::EnumValue>>& enumValues)
        : enum_(enum_), enumValues(enumValues){}

MayFail_<EnumDefinition>::MayFail_(const EnumDefinition& enumdef) {
    this->enum_ = enumdef.enum_;
    auto enumValues = std::vector<MayFail<MayFail_<EnumDefinition>::EnumValue>>();
    for (auto enumVal: enumdef.enumValues) {
        enumValues.push_back(MayFail<EnumValue>({enumVal.enumerator, wrap_expr(enumVal.enumerate)}));
    }
    this->enumValues = enumValues;

    this->_tokenLeadingNewlines = enumdef._tokenLeadingNewlines;
    this->_tokenIndentSpaces = enumdef._tokenIndentSpaces;
    this->_tokenLen = enumdef._tokenLen;
    this->_tokenTrailingNewlines = enumdef._tokenTrailingNewlines;
    this->_tokenId = enumdef._tokenId;
}

MayFail_<EnumDefinition>::operator EnumDefinition() const {
    auto enum_ = this->enum_;
    auto enumValues = std::vector<EnumDefinition::EnumValue>();
    for (auto enumVal: this->enumValues) {
        auto enumVal_ = enumVal.value();
        enumValues.push_back({enumVal_.enumerator, unwrap_expr(enumVal_.enumerate.value())});
    }
    auto enumdef = EnumDefinition{enum_, enumValues};

    enumdef._tokenLeadingNewlines = this->_tokenLeadingNewlines;
    enumdef._tokenIndentSpaces = this->_tokenIndentSpaces;
    enumdef._tokenLen = this->_tokenLen;
    enumdef._tokenTrailingNewlines = this->_tokenTrailingNewlines;
    enumdef._tokenId = this->_tokenId;

    return enumdef;
}
