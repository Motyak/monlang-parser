
#include <monlang-parser/parse.h>

#include <utils/iostream-utils.h>

void handleParsingResult(const ParsingResult&);

int main()
{
    while (true) {
        /* read */
        auto input_str = slurp_stdin();
        auto input_iss = std::istringstream(input_str);

        /* eval */
        auto parsingRes = parse(input_iss);

        /* print */
        handleParsingResult(parsingRes);
    }
}

void handleParsingResult(const ParsingResult& parsingRes) {
    if (parsingRes.status < 0) {
        std::cerr << "malformed input" << std::endl;
        return;
    }

    std::cerr << "correct input" << std::endl;
}
