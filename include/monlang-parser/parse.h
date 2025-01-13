#ifndef PARSE_H
#define PARSE_H

#include <monlang-parser/Tokens.h>

#include <monlang-LV1/Program.h>
#include <monlang-LV2/Program.h>

struct Source {
    std::string name; // e.g.: `file.txt`, `<stdin>`, default is `<str>`
    std::string text; // bytes

    Source() = default;
    Source(const std::string& text) : name("<str>"), text(text){} // TODO: .cpp
    Source(const std::string& name, const std::string& text) : name(name), text(text){} // TODO: .cpp
    operator std::string() const {return text;} // TODO: .cpp
};

struct ParsingResult {
    enum Status {
        // ...
        LV1_ERR = -2,
        LV2_ERR = -1,
        LV2_OK = 0
    }
    status;

    using Variant = std::variant<
        Malformed<Malformed_<LV1::Program>>,
        Malformed<Malformed_<LV2::Program>>,
        LV2::Program
    >;
    Variant variant;

    ParsingResult() = default;
    ParsingResult(const Status&, const Variant&);

    operator Variant() const {
        return variant;
    }

    Source _source; // for reporting eventual traceback
    std::optional<LV1::Program> _correctLV1 = std::nullopt;
    LV1Tokens _tokensLV1; // TODO: will become std::optional
    // std::optional<LV2Tokens> _tokensLV2 = std::nullopt; // TODO: impl
};

Malformed<Malformed_<LV1::Program>>
asMalformedLV1(const ParsingResult::Variant&);

Malformed<Malformed_<LV2::Program>>
asMalformedLV2(const ParsingResult::Variant&);

LV2::Program
asCorrectLV2(const ParsingResult::Variant&);

#ifdef PARSE_RESULT_TEST
#include <utils/assert-utils.h>
// g++ -x c++ -D PARSE_RESULT_TEST -c include/monlang-parser/parse.h -o main.o --std=c++23 -Wall -Wextra -I include
// g++ main.o lib/monlang-LV1/dist/monlang-LV1.a lib/monlang-LV2/dist/monlang-LV2.a -o main.elf
int main()
{
    using Status = ParsingResult::Status;
    auto parsingRes = ParsingResult{Status::LV1_ERR, Malformed<Malformed_<LV1::Program>>()};

    if (parsingRes.status < 0) {
        std::cout << "there was an error" << std::endl;
    }

    if (parsingRes.status == Status::LV1_ERR) {
        auto malformedLV1 = asMalformedLV1(parsingRes);
    }
    else if (parsingRes.status == Status::LV2_ERR) {
        auto malformedLV2 = asMalformedLV2(parsingRes);
    }
    else if (parsingRes.status == Status::LV2_OK) {
        auto correctLV2 = asCorrectLV2(parsingRes);
    }
    else {
        SHOULD_NOT_HAPPEN();
    }

    // asMalformedLV2(parsingRes); // std::bad_variant_access
}
#endif // PARSE_H_MAIN

ParsingResult parse(const Source& text);

#endif // PARSE_H
