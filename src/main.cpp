
#include <monlang-parser/parse.h>

#include <montree/PrintLV1.h>
#include <montree/PrintLV2.h>
#include <nlohmann/json.hpp>

#include <utils/assert-utils.h>
#include <utils/iostream-utils.h>
#include <utils/file-utils.h>
#include <utils/fs-utils.h>
#include <utils/str-utils.h>
#include <utils/loop-utils.h>
#include <utils/env-utils.h>

#define unless(x) if(!(x))

using ParsingResult::Status::LV1_ERR;
using ParsingResult::Status::LV2_ERR;
using ParsingResult::Status::LV2_OK;

static bool OUTPUT_MODE = false;
static std::string STDIN_SRCNAME = env_or_default("STDIN_SRCNAME", "<stdin>");

[[noreturn]] int repl_main(int argc, char* argv[]);
int stdinput_main(int argc, char* argv[]);
int fileinput_main(int argc, char* argv[]);

int main(int argc, char* argv[])
{

    if (auto options = second(split_in_two(argv[0], " -"))) {
        if (options->contains("o")) {
            OUTPUT_MODE = true;
        }
    }

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
    auto text = Source{STDIN_SRCNAME, input_str};
    auto parsingRes = parse(text);

    Print:
    handleParsingResult(parsingRes);

    Loop: goto Read
    ;
}

int stdinput_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    auto input_str = slurp_stdin(/*repeatable*/false);
    auto text = Source{STDIN_SRCNAME, input_str};
    auto parsingRes = parse(text);
    handleParsingResult(parsingRes);

    switch (parsingRes.status) {
        case LV2_OK: return 0;
        case LV1_ERR: return 101;
        case LV2_ERR: return 102;

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
    catch (const CantOpenFileException&) {
        std::cerr << "Failed to open file `" << filename << "`" << std::endl;
        return 103;
    }
    auto text = Source{filename, input_str};
    auto parsingRes = parse(text);

    handleParsingResult(parsingRes);

    switch (parsingRes.status) {
        case LV2_OK: return 0;
        case LV1_ERR: return 101;
        case LV2_ERR: return 102;

        default: SHOULD_NOT_HAPPEN();
    }
}

nlohmann::ordered_json buildJson(const TokenPosition&);
nlohmann::ordered_json buildJson(const Token&);
void serializeToJson(const Tokens&, std::ostream&);
void reportTraceback(std::ostream& out, const ParsingResult&);

void handleParsingResult(const ParsingResult& parsingRes) {
    if (OUTPUT_MODE) {
        if (parsingRes.status < LV2_OK) {
            reportTraceback(std::cout, parsingRes);
        }
        return;
    }

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
        std::ofstream("out/LV1.tokens.json", std::ios::trunc);
        std::ofstream("out/LV2.tokens.json", std::ios::trunc);
    }

    /* write out/LV1.ast.txt */
    {
        auto file = std::ofstream("out/LV1.ast.txt", std::ios::app);
        auto print_to_file = PrintLV1(file);
        // auto print_to_file = PrintLV1(std::cout); //debug
        MayFail<MayFail_<LV1::Program>> ast =
                parsingRes.status == LV1_ERR? asMalformedLV1(parsingRes)
                : MayFail<MayFail_<Program>>(parsingRes._correctLV1.value());
        print_to_file(ast);
    }

    /* write out/LV1.tokens.json */
    {
        auto file = std::ofstream("out/LV1.tokens.json", std::ios::app);
        serializeToJson(parsingRes._tokensLV1, file);
    }

    if (parsingRes.status > LV1_ERR)
    /* write out/LV2.ast.txt */
    {
        auto file = std::ofstream("out/LV2.ast.txt", std::ios::app);
        auto print_to_file = PrintLV2(file);
        // auto print_to_file = PrintLV2(std::cout); //debug
        MayFail<MayFail_<LV2::Program>> ast =
                parsingRes.status == LV2_ERR? asMalformedLV2(parsingRes)
                : MayFail<MayFail_<LV2::Program>>(asCorrectLV2(parsingRes));
        print_to_file(ast);
    }

    /* write out/LV2.tokens.json */
    {
        auto file = std::ofstream("out/LV2.tokens.json", std::ios::app);
        serializeToJson(parsingRes._tokensLV2, file);
    }

    if (parsingRes.status < LV2_OK)
    /* write out/traceback.txt */
    {
        auto file = std::ofstream("out/traceback.txt", std::ios::app);
        reportTraceback(file, parsingRes);
    }
}

nlohmann::ordered_json buildJson(const TokenPosition& tokenPos) {
    nlohmann::ordered_json json;
    json["i"] = tokenPos.i;
    json["line"] = tokenPos.line;
    json["column"] = tokenPos.column;
    return json;
}

nlohmann::ordered_json buildJson(const Token& token) {
    nlohmann::ordered_json json;
    json["name"] = token.name;
    json["start"] = buildJson(token.start);
    json["end"] = buildJson(token.end);
    json["is_malformed"] = token.is_malformed;
    json["err_start"] = buildJson(token.err_start);
    json["err_fmt"] = token.err_fmt;
    return json;
}

void serializeToJson(const Tokens& tokens, std::ostream& out) {
    nlohmann::ordered_json json;
    auto list = std::vector<nlohmann::ordered_json>{};
    for (auto token: tokens._vec) {
        list.push_back(buildJson(token));
    }
    json["list"] = list;
    
    auto traceback = std::vector<nlohmann::ordered_json>{};
    for (auto token: tokens.traceback) {
        traceback.push_back(buildJson(token));
    }
    json["traceback"] = traceback;

    out << json.dump(/*indent*/1, /*indent_char*/TAB) << std::endl;
}

void reportTraceback(std::ostream& out, const ParsingResult& parsingRes) {
    //TODO: other errors

    auto sourceLines = split(parsingRes._source, "\n");

    /*
        The first arrow `^` indicates the start of the malformed token,
        the second arrow `^` indicates the start of the error.
        e.g.:
        in/group_notok.txt:2:9: error: Malformed Term
            2 | (fds, sd]f)
              |       ^ ^ERR-103

        We prioritize printing the error_start arrow over the token_start arrow,
        as seen in the following cases:

        - If the two arrows point to the same location, you will only see one.
        e.g.:
        in/group_notok.txt:2:1: error: Malformed ProgramSentence
            2 | (fds, sd]f)
              | ^ERR-129
        (Here the ProgramSentence error is an inner Malformed ParenthesesGroup)

        - If the token start on a line above where error start,
        then only the second arrow will be printed (line where error starts).
        e.g. :
        in/group_notok.txt:2:1: error: Malformed Program
            2 | (fds, sd]f)
              | ^ERR-119
        (Program always start at line 1, but here we show line 2,
        where the Malformed ProgramSentence is located)

    */
    if (parsingRes.status == LV1_ERR) {
        TokenPosition err_start;
        LOOP for (auto token: parsingRes._tokensLV1.traceback) {
            if (__first_it) {
                /* will initialize err_start with token.err_start */
                err_start = token.err_start;
            }

            /* here will use err_start set in previous iteration, NEVER token.err_start */
            auto print_token_start_as_well = token.start.line == err_start.line && token.start.column != err_start.column;
            out << parsingRes._source.name << ":" << err_start.line << ":" << err_start.column << ": LV1 error: Malformed " << token.name << "\n";
            out << rjust(err_start.line, 5) << " | " << sourceLines.at(err_start.line - 1) << "\n";
            if (print_token_start_as_well) {
                auto the_two_arrows = "^" + std::string(err_start.column - token.start.column - 1, SPACE) + "^";
                out << "      | " << rjust(the_two_arrows, err_start.column) << token.err_fmt << "\n";
            }
            else {
                out << "      | " << rjust("^", err_start.column) << token.err_fmt << "\n";
            }
            err_start = token.start;
            ENDLOOP
        }
    }

    else if (parsingRes.status == LV2_ERR) {
        TokenPosition err_start;
        LOOP for (auto token: parsingRes._tokensLV2.traceback) {
            if (__first_it) {
                /* will initialize err_start with token.err_start */
                err_start = token.err_start;
            }

            /* here will use err_start set in previous iteration, NEVER token.err_start */
            auto print_token_start_as_well = token.start.line == err_start.line && token.start.column != err_start.column;
            out << parsingRes._source.name << ":" << err_start.line << ":" << err_start.column << ": LV2 error: Malformed " << token.name << "\n";
            out << rjust(err_start.line, 5) << " | " << sourceLines.at(err_start.line - 1) << "\n";
            if (print_token_start_as_well) {
                auto the_two_arrows = "^" + std::string(err_start.column - token.start.column - 1, SPACE) + "^";
                out << "      | " << rjust(the_two_arrows, err_start.column) << token.err_fmt << "\n";
            }
            else {
                out << "      | " << rjust("^", err_start.column) << token.err_fmt << "\n";
            }
            err_start = token.start;
            ENDLOOP
        }
    }

    else {
        SHOULD_NOT_HAPPEN();
    }
}
