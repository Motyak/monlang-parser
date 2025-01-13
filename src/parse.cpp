#include <monlang-parser/parse.h>
#include <monlang-parser/ReconstructLV1Tokens.h>

#include <utils/file-utils.h>

using ParsingResult::Status::LV1_ERR;
using ParsingResult::Status::LV2_ERR;
using ParsingResult::Status::LV2_OK;

ParsingResult::ParsingResult(const Status& status, const Variant& variant)
        : status(status), variant(variant){}

Malformed<Malformed_<LV1::Program>>
asMalformedLV1(const ParsingResult::Variant& result) {
    return std::get<0>(result);
}

Malformed<Malformed_<LV2::Program>>
asMalformedLV2(const ParsingResult::Variant& result) {
    return std::get<1>(result);
}

LV2::Program
asCorrectLV2(const ParsingResult::Variant& result) {
    return std::get<2>(result);
}

static std::vector<size_t> calculateNewlinesPos(const std::string& input) {
    std::vector<size_t> res;
    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == NEWLINE) {
            res.push_back(i);
        }
    }
    return res;
}

ParsingResult parse(const Source& text) {
    //TODO: check for UTF8_ERR // byte sequence cannot be interpreted as text (UTF8)
    //TODO: check for CHARSET_ERR // byte lies outside ASCII character sub-set comprising of LF(10), SPACE(32) and all visible characters (33-126)
    //TODO???: check for LINEFEED_ERR // non-empty line is missing a trailing LF character
    //  -> already catch by LV1 ProgramSentence
    auto newlinesPos = calculateNewlinesPos(text);
    auto iss = std::istringstream(text);

    auto progLV1 = consumeProgram(iss);
    if (progLV1.has_error()) {
        auto res = ParsingResult{LV1_ERR, progLV1};
        res._source = text;
        auto fill_tokens = ReconstructLV1Tokens(/*OUT*/res._tokensLV1, newlinesPos);
        fill_tokens(progLV1);
        return res;
    }

    auto correctLV1 = (LV1::Program)progLV1;
    auto backupCorrectLV1 = correctLV1;
    auto progLV2 = consumeProgram(correctLV1);
    if (progLV2.has_error()) {
        auto res = ParsingResult{LV2_ERR, progLV2};
        res._source = text;
        auto fill_tokens = ReconstructLV1Tokens(/*OUT*/res._tokensLV1, newlinesPos);
        fill_tokens(progLV1);
        res._correctLV1 = backupCorrectLV1;
        return res;
    }

    auto res = ParsingResult{LV2_OK, (LV2::Program)progLV2};
    res._source = text;
    auto fill_tokens = ReconstructLV1Tokens(/*OUT*/res._tokensLV1, newlinesPos);
    fill_tokens(progLV1);
    res._correctLV1 = backupCorrectLV1;
    return res;
}
