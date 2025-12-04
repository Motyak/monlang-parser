
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
#include <utils/unicode_eaw.h>

#include <future>

#define unless(x) if(!(x))

using ParsingResult::Status::LV1_ERR;
using ParsingResult::Status::LV2_ERR;
using ParsingResult::Status::LV2_OK;

static bool OUTPUT_MODE = false;
static auto SRCNAME = env_get("SRCNAME");

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
    auto text = Source{SRCNAME.value_or("<stdin>"), input_str};
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
    auto text = Source{SRCNAME.value_or("<stdin>"), input_str};
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
    auto text = Source{SRCNAME.value_or(filename), input_str};
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

    auto tasks = std::vector<std::future<void>>{};

    /* write out/LV1.tokens.json */ tasks.emplace_back(std::async(std::launch::async, [&parsingRes]()
    {
        auto file = std::ofstream("out/LV1.tokens.json", std::ios::app);
        serializeToJson(parsingRes._tokensLV1, file);
    }));

    /* write out/LV2.tokens.json */ tasks.emplace_back(std::async(std::launch::async, [&parsingRes]()
    {
        auto file = std::ofstream("out/LV2.tokens.json", std::ios::app);
        serializeToJson(parsingRes._tokensLV2, file);
    }));

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

    if (parsingRes.status < LV2_OK)
    /* write out/traceback.txt */
    {
        auto file = std::ofstream("out/traceback.txt", std::ios::app);
        reportTraceback(file, parsingRes);
    }

    for (auto& task: tasks) {
        task.get();
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
    list.reserve(tokens._vec.size());
    for (auto token: tokens._vec) {
        list.push_back(buildJson(token));
    }
    json["list"] = std::move(list);
    
    auto traceback = std::vector<nlohmann::ordered_json>{};
    for (auto token: tokens.traceback) {
        traceback.push_back(buildJson(token));
    }
    json["traceback"] = traceback;

    out << json.dump(/*indent*/1, /*indent_char*/TAB) << std::endl;
}

// default: ascii char (1 byte, neutral)
struct UnicharTableEntry {
    uint8_t size = 1; // between 1 and 4 bytes
    bool is_fullwidth = false;
};
std::vector<UnicharTableEntry> createUnicharTable(const std::string&);

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
            bool fullwidth_start_arrow = false;
            out << parsingRes._source.name << ":" << err_start.line << ":" << err_start.column << ": LV1 error: Malformed " << token.name << "\n";
            out << rjust(err_start.line, 5) << " | " << sourceLines.at(err_start.line - 1) << "\n";
            out << "      | ";
            auto token_start_lineIndex = token.start.i - std::accumulate(sourceLines.begin(), sourceLines.begin() + (token.start.line - 1), size_t(0), [](size_t a, const std::string& b){return a + b.size() + 1;});
            auto err_start_lineIndex = err_start.i - std::accumulate(sourceLines.begin(), sourceLines.begin() + (err_start.line - 1), size_t(0), [](size_t a, const std::string& b){return a + b.size() + 1;});
            auto unichar = createUnicharTable(sourceLines.at(err_start.line - 1));
            size_t i = 0;
            if (print_token_start_as_well) {
                while (i < token_start_lineIndex) {
                    out << (unichar.at(i).is_fullwidth? /* ideographic space (U+3000) */"　" : " ");
                    i += unichar.at(i).size;
                }
                if (i == token_start_lineIndex) {
                    out << (unichar.at(i).is_fullwidth && (fullwidth_start_arrow = true) ? /* fullwidth cirmculflex accent (U+FF3E) */"＾" : "^");
                    i += unichar.at(i).size;
                }
                // else SHOULD_NOT_HAPPEN(); // debug
            }
            while (i < err_start_lineIndex) {
                out << (unichar.at(i).is_fullwidth? /* ideographic space (U+3000) */"　" : " ");
                i += unichar.at(i).size;
            }
            if (i == err_start_lineIndex) {
                out << (fullwidth_start_arrow ? /* fullwidth cirmculflex accent (U+FF3E) */"＾" : "^");
            }
            // else SHOULD_NOT_HAPPEN(); // debug
            out << token.err_fmt << "\n";

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
            bool fullwidth_start_arrow = false;
            out << parsingRes._source.name << ":" << err_start.line << ":" << err_start.column << ": LV2 error: Malformed " << token.name << "\n";
            out << rjust(err_start.line, 5) << " | " << sourceLines.at(err_start.line - 1) << "\n";
            out << "      | ";
            auto token_start_lineIndex = token.start.i - std::accumulate(sourceLines.begin(), sourceLines.begin() + (token.start.line - 1), size_t(0), [](size_t a, const std::string& b){return a + b.size() + 1;});
            auto err_start_lineIndex = err_start.i - std::accumulate(sourceLines.begin(), sourceLines.begin() + (err_start.line - 1), size_t(0), [](size_t a, const std::string& b){return a + b.size() + 1;});
            auto unichar = createUnicharTable(sourceLines.at(err_start.line - 1));
            size_t i = 0;
            if (print_token_start_as_well) {
                while (i < token_start_lineIndex) {
                    out << (unichar.at(i).is_fullwidth? /* ideographic space (U+3000) */"　" : " ");
                    i += unichar.at(i).size;
                }
                if (i == token_start_lineIndex) {
                    out << (unichar.at(i).is_fullwidth && (fullwidth_start_arrow = true) ? /* fullwidth cirmculflex accent (U+FF3E) */"＾" : "^");
                    i += unichar.at(i).size;
                }
                // else SHOULD_NOT_HAPPEN(); // debug
            }
            while (i < err_start_lineIndex) {
                out << (unichar.at(i).is_fullwidth? /* ideographic space (U+3000) */"　" : " ");
                i += unichar.at(i).size;
            }
            if (i == err_start_lineIndex) {
                out << (fullwidth_start_arrow ? /* fullwidth cirmculflex accent (U+FF3E) */"＾" : "^");
            }
            // else SHOULD_NOT_HAPPEN(); // debug
            out << token.err_fmt << "\n";
            err_start = token.start;
            ENDLOOP
        }
    }

    else {
        SHOULD_NOT_HAPPEN();
    }
}

// table associating byte str index to its corresponding unichar entry
// (multi-byte character => duplicate entries)
std::vector<UnicharTableEntry> createUnicharTable(const std::string& str) {
    auto res = std::vector<UnicharTableEntry>();
    size_t i = 0; // byte str index
    uint8_t unicharBytes[4];
    uint32_t codepoint;
    while (i < str.size()) {
        unicharBytes[0] = str.at(i);
        unicharBytes[1] = unicharBytes[2] = unicharBytes[3] = 0;
        uint8_t size = unicharBytes[0] >> 5 == 0b110 ? 2
                : unicharBytes[0] >> 4 == 0b1110 ? 3
                : unicharBytes[0] >> 3 == 0b1111'0 ? 4
                : 1;
        unless (i + size - 1 < str.size()) goto FALLBACK;
        for (int nth_extra_byte = 1; nth_extra_byte <= size - 1; ++nth_extra_byte) {
            unicharBytes[nth_extra_byte] = str.at(i + nth_extra_byte);
            unless (unicharBytes[nth_extra_byte] >> 6 == 0b10) goto FALLBACK;
        }

        /* calculate if fullwidth based on unicharBytes, then propagate result as table entries */
        codepoint = size == 1? unicharBytes[0] & 0b0111'1111
                : size == 2? ((unicharBytes[0] & 0b0001'1111) << 6) + (unicharBytes[1] & 0b0011'1111)
                : size == 3? ((unicharBytes[0] & 0b0000'1111) << 12) + ((unicharBytes[1] & 0b0011'1111) << 6) + (unicharBytes[2] & 0b0011'1111)
                : /*size == 4*/ ((unicharBytes[0] & 0b0000'0111) << 18) + ((unicharBytes[1] & 0b0011'1111) << 12) + ((unicharBytes[2] & 0b0011'1111) << 6) + (unicharBytes[3] & 0b0011'1111);
        for (int j = 1; j <= size; ++j) {
            res.push_back({.size=size, .is_fullwidth=is_fullwidth(codepoint)});
            i += 1;
        }

        continue; // normal exit

        // push curr byte as non-utf8 byte and continue with next byte
        FALLBACK:
        res.push_back({.size=1, .is_fullwidth=false});
        i += 1;
    }

    return res;
}
