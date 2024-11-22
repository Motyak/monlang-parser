#include <montree/LV1AstBuilder.h>

#include <utils/mem-utils.h>
#include <utils/str-utils.h>
#include <utils/assert-utils.h>
#include <utils/vec-utils.h>
#include <utils/loop-utils.h>

#include <string>
#include <iostream>
#include <sstream>

namespace
{

const int SPACE = 32;
const int NEWLINE = 10;

enum LineType {
    standard = 0,
    INCR = 1,
    DECR = -1,
    END = -2,
};

struct Line {
    LineType type;
    std::string value;
};

struct TreeInputStream {
    int prev_tab_size;
    std::istringstream input;

    TreeInputStream(const std::string& input) : prev_tab_size(-1), input(input){}
};

Line consumeLine(TreeInputStream& tis) {
    std::string line = "";
    int line_tab_size = 0;
    
    char c;
    
    LOOP_TAB:
    c = tis.input.get();
    if (c == SPACE) {
        line_tab_size += 1;
        goto LOOP_TAB;
    }
    
    if (tis.prev_tab_size == -1) {
        tis.prev_tab_size = line_tab_size;
    }
    
    LOOP_LINE:
    if (c != NEWLINE && c != EOF) {
        line += std::string(1, c);
        c = tis.input.get();
        goto LOOP_LINE;
    }
    
    auto type = line == ""? END
              : line_tab_size > tis.prev_tab_size? INCR
              : line_tab_size < tis.prev_tab_size? DECR
              : standard;
    
    tis.prev_tab_size = line_tab_size;
    
    return Line{type, line};
}

Line peekLine(TreeInputStream& tis) {
    // save stream position
    std::streampos initial_pos = tis.input.tellg();

    auto res = consumeLine(tis);

    // reset flags if EOF reached etc..
    tis.input.clear();
    // restore stream position
    tis.input.seekg(initial_pos);

    return res;
}

} // end of anonymous namespace

///////////////////////////////////////////////////////////

#define ENTERING_BUILD_ROUTINE() auto& tis = *std::any_cast<TreeInputStream*>(this->tis)

LV1AstBuilder::LV1AstBuilder(const std::string& input): 
        tis(std::any_cast<TreeInputStream*>(move_to_heap(TreeInputStream{input}))){}

LV1::Ast LV1AstBuilder::buildAst() {
    ENTERING_BUILD_ROUTINE();

    const auto CANDIDATES = std::vector<std::string>{
        "Program",
        "ProgramSentence",
        "ProgramWord",
    };

    auto line = peekLine(tis);
    auto line_content = split(line.value, LINE_PREFIX).at(1);
    auto columns = split(line_content, ": ");

    auto first_candidate_found = std::string("");
    for (auto column: columns) {
        for (auto candidate: CANDIDATES) {
            if (column.starts_with(candidate)) {
                first_candidate_found = candidate;
                goto BREAK_DOUBLE_LOOP;
            }
        }
    }
    BREAK_DOUBLE_LOOP:
    ASSERT (first_candidate_found != "");

    if (first_candidate_found == "Program") {
        return buildProgram();
    }

    else if (first_candidate_found == "ProgramSentence") {
        return buildProgramSentence();
    }

    else if (first_candidate_found == "ProgramWord") {
        return buildProgramWord();
    }

    else {
        SHOULD_NOT_HAPPEN(); // bug
    }
}

LV1::Program LV1AstBuilder::buildProgram() {
    ENTERING_BUILD_ROUTINE();

    consumeLine(tis); // -> Program

    auto peekedLine = peekLine(tis);
    if (peekedLine.type == DECR) {
        SHOULD_NOT_HAPPEN(); // Program can't be contained
    }
    if (peekedLine.type != INCR) {
        return LV1::Program{};
    }
    
    std::vector<ProgramSentence> sentences;
    do {
        sentences.push_back(buildProgramSentence());
        peekedLine = peekLine(tis);
        if (peekedLine.type == INCR) {
            SHOULD_NOT_HAPPEN(); // shouldnt happen after a call to buildProgramSentence()
        }
    }
    until (peekedLine.type == DECR || peekedLine.type == END);

    return LV1::Program{sentences};
}

ProgramSentence LV1AstBuilder::buildProgramSentence() {
    ENTERING_BUILD_ROUTINE();

    consumeLine(tis); // -> ProgramSentence

    if (peekLine(tis).type != INCR) {
        SHOULD_NOT_HAPPEN(); // empty program sentence
    }
    
    std::vector<ProgramWord> programWords;
    Line peekedLine;
    do {
        programWords.push_back(buildProgramWord());
        peekedLine = peekLine(tis);
        if (peekedLine.type == INCR) {
            SHOULD_NOT_HAPPEN(); // shouldnt happen after a call to buildProgramWord()
        }
    }
    until (peekedLine.type == DECR || peekedLine.type == END);

    return ProgramSentence{programWords};
}

ProgramWord LV1AstBuilder::buildProgramWord() {
    ENTERING_BUILD_ROUTINE();

    const auto CANDIDATES = std::vector<std::string>{
        "Atom",
    };

    auto line = peekLine(tis); // -> ProgramWord...
    auto line_content = split(line.value, LINE_PREFIX).at(1);
    auto columns = split(line_content, ": ");

    auto first_candidate_found = std::string("");
    for (auto column: columns) {
        if (vec_contains(CANDIDATES, column)) {
            first_candidate_found = column;
            break;
        }
    }
    ASSERT (first_candidate_found != "");

    if (first_candidate_found == "Atom") {
        return move_to_heap(buildAtom());
    }

    else {
        SHOULD_NOT_HAPPEN(); // bug
    }
}

Atom LV1AstBuilder::buildAtom() {
    ENTERING_BUILD_ROUTINE();

    auto line = consumeLine(tis); // -> ... Atom: `<atom_val>`
    auto atom_val = split(line.value, "`").at(1);

    if (peekLine(tis).type == INCR) {
        SHOULD_NOT_HAPPEN();
    }

    return Atom{atom_val};
}

Term LV1AstBuilder::buildTerm() {

}

Word LV1AstBuilder::buildWord() {

}

SquareBracketsTerm LV1AstBuilder::buildSquareBracketsTerm() {

}

Association LV1AstBuilder::buildAssociation() {

}

PostfixParenthesesGroup LV1AstBuilder::buildPostfixParenthesesGroup() {

}

PostfixSquareBracketsGroup LV1AstBuilder::buildPostfixSquareBracketsGroup() {

}

ParenthesesGroup LV1AstBuilder::buildParenthesesGroup() {

}

SquareBracketsGroup LV1AstBuilder::buildSquareBracketsGroup() {

}

CurlyBracketsGroup LV1AstBuilder::buildCurlyBracketsGroup() {

}




#ifdef TREE_INPUT_STREAM_MAIN
// g++ -D TREE_INPUT_STREAM_MAIN -o tis_main.elf src/LV1AstBuilder.cpp --std=c++23 -Wall -Wextra -I include
int main()
{
    auto tis = TreeInputStream{
        "salut à tous\n"
        "les moches"
    };
    peekLine(tis);
    auto line1 = consumeLine(tis);
    std::cout << line1.type << "," << line1.value << std::endl;
    auto line2 = consumeLine(tis);
    std::cout << line2.type << "," << line2.value << std::endl;
    auto line3 = consumeLine(tis);
    std::cout << line3.type << "," << line3.value << std::endl;
}
#endif // TREE_INPUT_STREAM_MAIN
