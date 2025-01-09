
#include <monlang-parser/parse.h>

#include <montree/PrintLV1.h>
#include <montree/PrintLV2.h>

#include <utils/assert-utils.h>
#include <utils/iostream-utils.h>
#include <utils/file-utils.h>
#include <utils/fs-utils.h>

#include <fstream>

#define unless(x) if(!(x))

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
    auto parsingRes = parseStr(input_str);

    Print:
    handleParsingResult(parsingRes);

    Loop: goto Read
    ;
}

int stdinput_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    auto input_str = slurp_stdin(/*repeatable*/false);
    auto parsingRes = parseStr(input_str);
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

    ParsingResult parsingRes;
    try {
        parsingRes = parseFile(filename);
    }
    catch (const CantOpenFileException&) {
        std::cerr << "Failed to open file `" << filename << "`" << std::endl;
        return 13;
    }

    handleParsingResult(parsingRes);

    switch (parsingRes.status) {
        case LV2_OK: return 0;
        case LV1_ERR: return 11;
        case LV2_ERR: return 12;

        default: SHOULD_NOT_HAPPEN();
    }
}

void handleParsingResult(const ParsingResult& parsingRes) {
    if (parsingRes.status < 0) {
        std::cerr << "malformed input" << std::endl;
    }
    else {
        std::cerr << "correct input" << std::endl;
    }

    unless (dir_exists("out")) return;

    /* empty all out/ files */
    {
        std::ofstream("out/LV1.ast.txt", std::ios::trunc);
        std::ofstream("out/LV2.ast.txt", std::ios::trunc);
        std::ofstream("out/traceback.txt", std::ios::trunc);
    }

    // if (parsingRes.status > CHARSET_ERR)
    /* write out/LV1.ast.txt */
    {
        auto file = std::ofstream("out/LV1.ast.txt", std::ios::app);
        auto print_to_file = PrintLV1(file);
        MayFail<MayFail_<LV1::Program>> ast =
                parsingRes.status == LV1_ERR? asMalformedLV1(parsingRes)
                : MayFail(MayFail_<Program>(parsingRes._correctLV1.value()));
        print_to_file(ast);
    }

    if (parsingRes.status > LV1_ERR)
    /* write out/LV2.ast.txt */
    {
        auto file = std::ofstream("out/LV2.ast.txt", std::ios::app);
        auto print_to_file = PrintLV2(file);
        MayFail<MayFail_<LV2::Program>> ast =
                parsingRes.status == LV2_ERR? asMalformedLV2(parsingRes)
                : MayFail(MayFail_<LV2::Program>(asCorrectLV2(parsingRes)));
        print_to_file(ast);
    }

    if (parsingRes.status < LV2_OK)
    /* write out/traceback.txt */
    {
        //TODO: later
    }
}
