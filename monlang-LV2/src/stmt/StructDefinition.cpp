#include <monlang-LV2/stmt/StructDefinition.h>

/* impl only */

#include <monlang-LV2/Expression.h>

#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/ProgramSentence.h>
/* require knowing all words for token_len() */
#include <monlang-LV1/ast/token_len.h>

#include <utils/assert-utils.h>

#define unless(x) if(!(x))

#define SET_TOKEN_FIELDS(typeDef, sentence) \
    typeDef._tokenLeadingNewlines = sentence._tokenLeadingNewlines; \
    typeDef._tokenIndentSpaces = sentence._tokenIndentSpaces; \
    typeDef._tokenLen = sentence._tokenLen; \
    typeDef._tokenTrailingNewlines = sentence._tokenTrailingNewlines

#define SET_MALFORMED_TOKEN_FIELDS(malformed, sentence) \
    malformed.val._tokenLeadingNewlines = sentence._tokenLeadingNewlines; \
    malformed.val._tokenIndentSpaces = sentence._tokenIndentSpaces

#define SET_FIELD_TOKEN_FIELDS(field, sentence) \
    field._tokenLeadingNewlines = sentence._tokenLeadingNewlines; \
    field._tokenLen = sentence._tokenLen; \
    field._tokenTrailingNewlines = sentence._tokenTrailingNewlines

#define SET_MALFORMED_FIELD_TOKEN_FIELDS(malformed_field, sentence_) \
    malformed_field.val._tokenLeadingNewlines = sentence_._tokenLeadingNewlines; \
    malformed_field.val._tokenTrailingNewlines = sentence_._tokenTrailingNewlines

// sum token len for all words preceding the nth word..
// ..and add it to error offset
#define SET_NTH_WORD_ERR_OFFSET(error, nth) \
    auto err_offset = size_t(0); \
    for (size_t i = 0; i < nth - 1; ++i) { \
        err_offset += token_len(sentence.programWords[i]); \
        err_offset += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE); \
    } \
    error._info["err_offset"] = err_offset

#define SET_FIELD_NAME_ERR_OFFSET(error) \
    auto err_offset = size_t(0); \
    err_offset += token_len(sentence_.programWords[0]); \
    err_offset += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE); \
    error._info["err_offset"] = err_offset; \

const std::string StructDefinition::KEYWORD = "struct";

bool peekStructDefinition(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 1) {
        return false;
    }

    auto pw = sentence.programWords[0];
    unless (std::holds_alternative<Atom*>(pw)) {
        return false;
    }

    auto atom = *std::get<Atom*>(pw);
    return atom.value == StructDefinition::KEYWORD;
}

static ProgramSentence consumeSentence(LV1::Program&);

MayFail<MayFail_<StructDefinition>> consumeStructDefinition(LV1::Program& prog) {
    auto sentence = consumeSentence(prog);


    unless (sentence.programWords.size() >= 2) {
        auto malformed = Malformed(MayFail_<StructDefinition>{Symbol(), {}}, ERR(421));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    ASSERT (holds_word(sentence.programWords[1]));
    auto word = get_word(sentence.programWords[1]);
    auto is_an_atom = std::holds_alternative<Atom*>(word);
    auto expr = buildExpression((Term)word);
    unless (std::holds_alternative<Symbol*>(expr.val)) {
        auto error = ERR(422);
        SET_NTH_WORD_ERR_OFFSET(error, /*nth*/2);
        auto malformed = Malformed(MayFail_<StructDefinition>{Symbol(), {}}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    auto struct_ = *std::get<Symbol*>(expr.val);

    unless (is_an_atom) {
        auto error = ERR(423);
        SET_NTH_WORD_ERR_OFFSET(error, /*nth*/2);
        auto malformed = Malformed(MayFail_<StructDefinition>{Symbol(), {}}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }


    unless (sentence.programWords.size() >= 3) {
        auto malformed = Malformed(MayFail_<StructDefinition>{struct_, {}}, ERR(424));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    
    auto block_as_pw = sentence.programWords[2];
    ASSERT (holds_word(block_as_pw));
    auto blockAsWord = get_word(block_as_pw);

    unless (std::holds_alternative<CurlyBracketsGroup*>(blockAsWord)) {
        auto error = ERR(425);
        SET_NTH_WORD_ERR_OFFSET(error, 3);
        auto malformed = Malformed(MayFail_<StructDefinition>{struct_, {}}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }
    auto block = *std::get<CurlyBracketsGroup*>(blockAsWord);

    if (block.term) {
        auto error = ERR(426);
        SET_NTH_WORD_ERR_OFFSET(error, 3);
        auto malformed = Malformed(MayFail_<StructDefinition>{struct_, {}}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/sentence);
        return malformed;
    }

    std::vector<MayFail<StructDefinition::Field>> fields;
    for (auto sentence_: block.sentences) {
        Symbol type, name;

        /* type */
        {
            ASSERT (sentence_.programWords.size() >= 1);
            ASSERT (holds_word(sentence_.programWords[0]));
            auto word = get_word(sentence_.programWords[0]);
            auto is_an_atom = std::holds_alternative<Atom*>(word);
            auto expr = buildExpression((Term)word);
            unless (std::holds_alternative<Symbol*>(expr.val)) {
                auto malformed_field = Malformed(StructDefinition::Field{type, name}, ERR(431));
                SET_MALFORMED_FIELD_TOKEN_FIELDS(malformed_field, sentence_);
                fields.push_back(malformed_field);
                auto malformed = Malformed(MayFail_<StructDefinition>{struct_, fields}, ERR(427));
                SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
                return malformed;
            }
            unless (is_an_atom) {
                auto malformed_field = Malformed(StructDefinition::Field{type, name}, ERR(432));
                SET_MALFORMED_FIELD_TOKEN_FIELDS(malformed_field, sentence_);
                fields.push_back(malformed_field);
                auto malformed = Malformed(MayFail_<StructDefinition>{struct_, fields}, ERR(427));
                SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
                return malformed;
            }
            type = *std::get<Symbol*>(expr.val);
        }

        unless (sentence_.programWords.size() >= 2) {
            auto malformed_field = Malformed(StructDefinition::Field{type, name}, ERR(433));
            SET_MALFORMED_FIELD_TOKEN_FIELDS(malformed_field, sentence_);
            fields.push_back(malformed_field);
            auto malformed = Malformed(MayFail_<StructDefinition>{struct_, fields}, ERR(427));
            SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
            return malformed;
        }

        /* name */
        {
            ASSERT (sentence_.programWords.size() >= 1);
            ASSERT (holds_word(sentence_.programWords[1]));
            auto word = get_word(sentence_.programWords[1]);
            auto is_an_atom = std::holds_alternative<Atom*>(word);
            auto expr = buildExpression((Term)word);
            unless (std::holds_alternative<Symbol*>(expr.val)) {
                auto error = ERR(434);
                SET_FIELD_NAME_ERR_OFFSET(error);
                auto malformed_field = Malformed(StructDefinition::Field{type, name}, error);
                SET_MALFORMED_FIELD_TOKEN_FIELDS(malformed_field, sentence_);
                fields.push_back(malformed_field);
                auto malformed = Malformed(MayFail_<StructDefinition>{struct_, fields}, ERR(427));
                SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
                return malformed;
            }
            unless (is_an_atom) {
                auto error = ERR(435);
                SET_FIELD_NAME_ERR_OFFSET(error);
                auto malformed_field = Malformed(StructDefinition::Field{type, name}, error);
                SET_MALFORMED_FIELD_TOKEN_FIELDS(malformed_field, sentence_);
                fields.push_back(malformed_field);
                auto malformed = Malformed(MayFail_<StructDefinition>{struct_, fields}, ERR(427));
                SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
                return malformed;
            }
            name = *std::get<Symbol*>(expr.val);
        }

        auto field = StructDefinition::Field{type, name};
        SET_FIELD_TOKEN_FIELDS(field, sentence_);
        fields.push_back(field);
    }

    auto structDef = MayFail_<StructDefinition>{struct_, fields};
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

StructDefinition::StructDefinition(const Symbol& struct_, const std::vector<Field>& fields)
        : struct_(struct_), fields(fields){}

StructDefinition::Field::Field(const Symbol& type, const Symbol& name)
        : type(type), name(name){}

MayFail_<StructDefinition>::MayFail_(const Symbol& struct_, const std::vector<MayFail<StructDefinition::Field>>& fields)
        : struct_(struct_), fields(fields){}

MayFail_<StructDefinition>::MayFail_(const StructDefinition& structdef) {
    this->struct_ = structdef.struct_;
    auto fields = std::vector<MayFail<StructDefinition::Field>>();
    for (auto f: structdef.fields) {
        fields.push_back(f);
    }
    this->fields = fields;

    this->_tokenLeadingNewlines = structdef._tokenLeadingNewlines;
    this->_tokenIndentSpaces = structdef._tokenIndentSpaces;
    this->_tokenLen = structdef._tokenLen;
    this->_tokenTrailingNewlines = structdef._tokenTrailingNewlines;
    this->_tokenId = structdef._tokenId;
}

MayFail_<StructDefinition>::operator StructDefinition() const {
    auto struct_ = this->struct_;
    auto fields = std::vector<StructDefinition::Field>();
    for (auto e: this->fields) {
        fields.push_back(e.value());
    }
    auto structdef = StructDefinition{struct_, fields};

    structdef._tokenLeadingNewlines = this->_tokenLeadingNewlines;
    structdef._tokenIndentSpaces = this->_tokenIndentSpaces;
    structdef._tokenLen = this->_tokenLen;
    structdef._tokenTrailingNewlines = this->_tokenTrailingNewlines;
    structdef._tokenId = this->_tokenId;

    return structdef;
}
