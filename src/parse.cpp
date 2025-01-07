#include <monlang-parser/parse.h>

#include <utils/file-utils.h>

using ParsingResult::Status::LV1_ERR;
using ParsingResult::Status::LV2_ERR;
using ParsingResult::Status::LV2_OK;

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

ParsingResult parse(std::istringstream& input) {
    auto progLV1 = consumeProgram(input);
    if (progLV1.has_error()) {
        return ParsingResult{LV1_ERR, progLV1};
    }

    auto correctLV1 = (LV1::Program)progLV1;
    auto progLV2 = consumeProgram(correctLV1);
    if (progLV2.has_error()) {
        return ParsingResult{LV2_ERR, progLV2};
    }

    return ParsingResult{LV2_OK, (LV2::Program)progLV2};
}

ParsingResult parse(const std::string& filename) {
    auto input_str = slurp_file(filename);

    auto input_iss = std::istringstream(input_str);

    auto parsingRes = parse(input_iss);
    parsingRes.filename = filename;

    return parsingRes;
}
