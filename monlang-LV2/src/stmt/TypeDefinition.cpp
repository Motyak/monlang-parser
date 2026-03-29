#include <monlang-LV2/stmt/TypeDefinition.h>

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

const std::string TypeDefinition::KEYWORD = "type";

bool peekTypeDefinition(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 1) {
        return false;
    }

    auto pw = sentence.programWords[0];
    unless (std::holds_alternative<Atom*>(pw)) {
        return false;
    }

    auto atom = *std::get<Atom*>(pw);
    return atom.value == TypeDefinition::KEYWORD;
}

static ProgramSentence consumeSentence(LV1::Program&);

MayFail<TypeDefinition> consumeTypeDefinition(LV1::Program& prog) {
    auto sentence = consumeSentence(prog);


    unless (sentence.programWords.size() >= 2) {
        auto malformed = Malformed(TypeDefinition{Symbol(), {}}, ERR(411));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    ASSERT (holds_word(sentence.programWords[1]));
    auto word = get_word(sentence.programWords[1]);
    auto is_an_atom = std::holds_alternative<Atom*>(word);
    auto expr = buildExpression((Term)word);
    unless (std::holds_alternative<Symbol*>(expr.val)) {
        auto error = ERR(412);
        SET_NTH_WORD_ERR_OFFSET(error, /*nth*/2);
        auto malformed = Malformed(TypeDefinition{Symbol(), {}}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    auto type = *std::get<Symbol*>(expr.val);

    unless (is_an_atom) {
        auto error = ERR(413);
        SET_NTH_WORD_ERR_OFFSET(error, /*nth*/2);
        auto malformed = Malformed(TypeDefinition{Symbol(), {}}, error);
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }


    unless (sentence.programWords.size() >= 3) {
        auto malformed = Malformed(TypeDefinition{type, {}}, ERR(414));
        SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
        return malformed;
    }
    auto programWords = std::vector<ProgramWord>(
        sentence.programWords.begin() + 2,
        sentence.programWords.end()
    );

    std::vector<Symbol> subtypes;
    for (size_t i = 0; i < programWords.size(); ++i) {
        ASSERT (holds_word(programWords[i]));
        auto word = get_word(programWords[i]);

        auto is_an_atom = std::holds_alternative<Atom*>(word);
        auto expr = buildExpression((Term)word);
        unless (std::holds_alternative<Symbol*>(expr.val)) {
            auto error = ERR(415);
            auto nth = i + 2 +1;
            SET_NTH_WORD_ERR_OFFSET(error, nth);
            auto malformed = Malformed(TypeDefinition{type, subtypes}, error);
            SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
            return malformed;
        }
        unless (is_an_atom) {
            auto error = ERR(416);
            auto nth = i + 2 +1;
            SET_NTH_WORD_ERR_OFFSET(error, nth);
            auto malformed = Malformed(TypeDefinition{type, subtypes}, error);
            SET_MALFORMED_TOKEN_FIELDS(malformed, /*from*/ sentence);
            return malformed;
        }
        subtypes.push_back(*std::get<Symbol*>(expr.val));
    }

    auto typeDef = TypeDefinition{type, subtypes};
    SET_TOKEN_FIELDS(typeDef, /*from*/ sentence);
    return typeDef;
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

TypeDefinition::TypeDefinition(const Symbol& type, const std::vector<Symbol>& subtypes)
        : type(type), subtypes(subtypes){}
