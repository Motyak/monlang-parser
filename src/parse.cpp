#include <monlang-parser/parse.h>
#include <monlang-parser/ReconstructLV1Tokens.h>
#include <monlang-parser/ReconstructLV2Tokens.h>

#include <utils/file-utils.h>

using ParsingResult::Status::LV1_ERR;
using ParsingResult::Status::LV2_ERR;
using ParsingResult::Status::LV2_OK;

static std::vector<size_t> calculateNewlinesPos(const std::string& input) {
    std::vector<size_t> res;
    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == NEWLINE) {
            res.push_back(i);
        }
    }
    return res;
}

/*
    returns a sequence of unichar (2+ byte utf8 character)
    position, along with their extra bytes count
*/
static std::vector<std::pair<size_t, unsigned>>
calculateUnicharBytesPos(const std::string& input) {
    std::vector<std::pair<size_t, unsigned>> res;
    size_t pos = 0;
    unsigned count = 0;
    for (size_t i = 0; i < input.size(); ++i) {
        if ((input[i] & 0b1100'0000) == 0b10 << 6) {
            count += 1;
        }
        else if (count > 0) {
            res.push_back({pos, count});
            /* reset */
            pos = i;
            count = 0;
        }
        else {
            pos = i;
        }
    }
    return res;
}

ParsingResult parse(const Source& text) {
    auto newlinesPos = calculateNewlinesPos(text);
    auto unicharBytesPos = calculateUnicharBytesPos(text);
    auto iss = std::istringstream(text);

    auto progLV1 = consumeProgram(iss);
    Tokens tokensLV1;
    {
        auto fill_tokensLV1 = ReconstructLV1Tokens(/*OUT*/tokensLV1, newlinesPos, unicharBytesPos);
        fill_tokensLV1(/*OUT*/progLV1);
    }
    if (progLV1.has_error()) {
        auto res = ParsingResult{LV1_ERR, progLV1};
        res._tokensLV1 = tokensLV1;
        res._source = text;
        return res;
    }

    auto correctLV1 = (LV1::Program)progLV1;
    auto backupCorrectLV1 = correctLV1;
    auto progLV2 = consumeProgram(/*OUT*/correctLV1);
    Tokens tokensLV2;
    {
        auto fill_tokensLV2 = ReconstructLV2Tokens(/*OUT*/tokensLV2, newlinesPos, unicharBytesPos);
        fill_tokensLV2(progLV2);
    }
    if (progLV2.has_error()) {
        auto res = ParsingResult{LV2_ERR, progLV2};
        res._correctLV1 = backupCorrectLV1;
        res._tokensLV1 = tokensLV1;
        res._tokensLV2 = tokensLV2;
        res._source = text;
        return res;
    }

    auto res = ParsingResult{LV2_OK, (LV2::Program)progLV2};
    res._correctLV1 = backupCorrectLV1;
    res._tokensLV1 = tokensLV1;
    res._tokensLV2 = tokensLV2;
    res._source = text;
    return res;
}

Source::Source(const std::string& text) : name("<str>"), text(text){}

Source::Source(const std::string& name, const std::string& text) : name(name), text(text){}

Source::operator std::string() const {
    return text;
}

ParsingResult::ParsingResult(const Status& status, const Variant& variant)
        : status(status), variant(variant){}

ParsingResult::operator Variant() const {
    return variant;
}

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
