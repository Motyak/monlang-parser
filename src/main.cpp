
#include <monlang-parser/parse.h>

#include <utils/iostream-utils.h>
#include <utils/file-utils.h>

using ParsingResult::Status::LV1_ERR;
using ParsingResult::Status::LV2_ERR;
using ParsingResult::Status::LV2_OK;

[[noreturn]] int repl_main(int argc, char* argv[]);
int stdinput_main(int argc, char* argv[]);
int fileinput_main(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    /* delegate main based on execution mode */

    if (argc == 1) {
        return repl_main(argc, argv);
    }

    if (*argv[1] == '-') {
        return stdinput_main(argc, argv);
    }

    return fileinput_main(argc, argv);
}

void handleParsingResult(const ParsingResult&);

int repl_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    Read:
    auto input_str = slurp_stdin(/*repeatable*/true);

    Eval:
    auto input_iss = std::istringstream(input_str);
    auto parsingRes = parse(input_iss);

    Print:
    handleParsingResult(parsingRes);

    Loop: goto Read
    ;
}

int stdinput_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    auto input_str = slurp_stdin(/*repeatable*/false);

    auto input_iss = std::istringstream(input_str);
    auto parsingRes = parse(input_iss);

    handleParsingResult(parsingRes);

    switch (parsingRes.status) {
        case LV2_OK: return 0;
        case LV1_ERR: return 11;
        case LV2_ERR: return 12;

        default: SHOULD_NOT_HAPPEN();
    }
}

int fileinput_main(int argc, char* argv[]) {
    (void)argc;
    const auto filename = argv[1];

    std::string input_str;
    try {
        input_str = slurp_file(filename);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 13;
    }

    auto input_iss = std::istringstream(input_str);
    auto parsingRes = parse(input_iss);

    handleParsingResult(parsingRes);

    switch (parsingRes.status) {
        case LV2_OK: return 0;
        case LV1_ERR: return 1;
        case LV2_ERR: return 2;

        default: SHOULD_NOT_HAPPEN();
    }
}

void handleParsingResult(const ParsingResult& parsingRes) {
    if (parsingRes.status < 0) {
        std::cerr << "malformed input" << std::endl;
        return;
    }

    std::cerr << "correct input" << std::endl;
}
