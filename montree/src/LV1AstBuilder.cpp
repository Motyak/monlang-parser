#include <montree/LV1AstBuilder.h>

#include <utils/mem-utils.h>
#include <utils/str-utils.h>
#include <utils/assert-utils.h>
#include <utils/vec-utils.h>
#include <utils/variant-utils.h>

#include <string>
#include <iostream>
#include <sstream>

#define until(x) while(!(x))
#define unless(x) if(!(x))

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
    int nestingLevel;
    std::string value;
};

struct TreeInputStream {
    int prev_tab_size;
    int prev_nesting_level = 0;
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
    
    LineType type;
    int nesting_level = tis.prev_nesting_level;
    if (line == "") {
        type = END;
    }
    else if (line_tab_size > tis.prev_tab_size) {
        type = INCR;
        nesting_level = tis.prev_nesting_level + 1;
    }
    else if (line_tab_size < tis.prev_tab_size) {
        ASSERT (tis.prev_nesting_level > 0);
        type = DECR;
        auto indent_size = tis.prev_tab_size / tis.prev_nesting_level;
        nesting_level = line_tab_size / indent_size;
    }
    else {
        type = standard;
    }
    
    tis.prev_tab_size = line_tab_size;
    tis.prev_nesting_level = nesting_level;
    
    return Line{type, nesting_level, line};
}

Line peekLine(TreeInputStream& tis) {
    // save stream position
    std::streampos initial_pos = tis.input.tellg();
    // save prev_tab_size
    int prev_tab_size = tis.prev_tab_size;
    // save prev_nesting_level
    int prev_nesting_level = tis.prev_nesting_level;

    auto res = consumeLine(tis);

    // reset flags if EOF reached etc..
    tis.input.clear();
    // restore stream position
    tis.input.seekg(initial_pos);
    // restore prev_tab_size
    tis.prev_tab_size = prev_tab_size;
    // restore prev_nesting_level
    tis.prev_nesting_level = prev_nesting_level;

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
        "ProgramSentence",
        "ProgramWord",
        "Program", // position here matters because 'starts_with()'
        "Term",
        "Word",
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

    else if (first_candidate_found == "Term") {
        return buildTerm();
    }

    else if (first_candidate_found == "Word") {
        return buildWord();
    }

    else {
        SHOULD_NOT_HAPPEN(); // bug
    }
}

LV1::Program LV1AstBuilder::buildProgram() {
    ENTERING_BUILD_ROUTINE();

    // we allow sequence of programs in one same stream
    auto parentNestingLevel = consumeLine(tis).nestingLevel; // -> Program

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
    until (peekedLine.nestingLevel <= parentNestingLevel || peekedLine.type == END);

    return LV1::Program{sentences};
}

ProgramSentence LV1AstBuilder::buildProgramSentence() {
    ENTERING_BUILD_ROUTINE();

    auto parentNestingLevel = consumeLine(tis).nestingLevel; // -> ProgramSentence

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
    until (peekedLine.nestingLevel <= parentNestingLevel || peekedLine.type == END);

    return ProgramSentence{programWords};
}

ProgramWord LV1AstBuilder::buildProgramWord() {
    ENTERING_BUILD_ROUTINE();

    const auto CANDIDATES = std::vector<std::string>{
        "Atom",
        "PostfixParenthesesGroup",
        "Association",
        "CurlyBracketsGroup",
        "ParenthesesGroup",
        "SquareBracketsTerm",
        "SquareBracketsGroup",
    };

    auto line = peekLine(tis); // -> ProgramWord...
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

    if (first_candidate_found == "Atom") {
        return move_to_heap(buildAtom());
    }

    else if (first_candidate_found == "PostfixParenthesesGroup") {
        return move_to_heap(buildPostfixParenthesesGroup());
    }

    else if (first_candidate_found == "Association") {
        return move_to_heap(buildAssociation());
    }

    else if (first_candidate_found == "CurlyBracketsGroup") {
        return move_to_heap(buildCurlyBracketsGroup());
    }

    else if (first_candidate_found == "ParenthesesGroup") {
        return move_to_heap(buildParenthesesGroup());
    }

    else if (first_candidate_found == "SquareBracketsTerm") {
        return move_to_heap(buildSquareBracketsTerm());
    }

    else if (first_candidate_found == "SquareBracketsGroup") {
        return move_to_heap(buildSquareBracketsGroup());
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

Quotation LV1AstBuilder::buildQuotation() {
    ENTERING_BUILD_ROUTINE();

    auto line = consumeLine(tis); // -> ... Quotation: `<quoted>`
    auto quoted = split(line.value, "`").at(1);

    if (peekLine(tis).type == INCR) {
        SHOULD_NOT_HAPPEN();
    }

    return Quotation{quoted};
}

Term LV1AstBuilder::buildTerm() {
    ENTERING_BUILD_ROUTINE();

    auto parentNestingLevel = consumeLine(tis).nestingLevel; // -> Term

    if (peekLine(tis).type != INCR) {
        SHOULD_NOT_HAPPEN(); // empty term
    }

    std::vector<Word> words;
    Line peekedLine;
    do {
        words.push_back(buildWord());
        peekedLine = peekLine(tis);
        if (peekedLine.type == INCR) {
            SHOULD_NOT_HAPPEN(); // shouldnt happen after a call to buildWord()
        }
    }
    until (peekedLine.nestingLevel <= parentNestingLevel || peekedLine.type == END);

    return Term{words};
}

Word LV1AstBuilder::buildWord() {
    ENTERING_BUILD_ROUTINE();

    const auto CANDIDATES = std::vector<std::string>{
        "Atom",
        "Quotation",
        "ParenthesesGroup",
        "SquareBracketsGroup",
        "CurlyBracketsGroup",
        "Association",
        "PostfixParenthesesGroup",
        "PostfixSquareBracketsGroup",
        "Path",
    };

    auto line = peekLine(tis); // -> Word...
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

    if (first_candidate_found == "Atom") {
        return move_to_heap(buildAtom());
    }

    else if (first_candidate_found == "Quotation") {
        return move_to_heap(buildQuotation());
    }

    else if (first_candidate_found == "ParenthesesGroup") {
        return move_to_heap(buildParenthesesGroup());
    }

    else if (first_candidate_found == "SquareBracketsGroup") {
        return move_to_heap(buildSquareBracketsGroup());
    }

    else if (first_candidate_found == "CurlyBracketsGroup") {
        return move_to_heap(buildCurlyBracketsGroup());
    }

    else if (first_candidate_found == "Association") {
        return move_to_heap(buildAssociation());
    }

    else if (first_candidate_found == "PostfixParenthesesGroup") {
        return move_to_heap(buildPostfixParenthesesGroup());
    }

    else if (first_candidate_found == "PostfixSquareBracketsGroup") {
        return move_to_heap(buildPostfixSquareBracketsGroup());
    }

    else if (first_candidate_found == "Path") {
        return move_to_heap(buildPath());
    }

    else {
        SHOULD_NOT_HAPPEN(); // bug
    }
}

ParenthesesGroup LV1AstBuilder::buildParenthesesGroup() {
    ENTERING_BUILD_ROUTINE();

    auto line = consumeLine(tis); // -> ... ParenthesesGroup
    auto peekedLine = peekLine(tis);

    if (peekedLine.type != INCR) {
        return ParenthesesGroup{};
    }

    std::vector<Term> terms;
    do {
        terms.push_back(buildTerm());
        peekedLine = peekLine(tis);
        if (peekedLine.type == INCR) {
            SHOULD_NOT_HAPPEN(); // shouldnt happen after a call to buildTerm()
        }
    }
    until (peekedLine.type == DECR || peekedLine.type == END);

    return ParenthesesGroup{terms};
}

SquareBracketsGroup LV1AstBuilder::buildSquareBracketsGroup() {
    ENTERING_BUILD_ROUTINE();

    auto parentNestingLevel = consumeLine(tis).nestingLevel; // -> ... SquareBracketsGroup
    auto peekedLine = peekLine(tis);

    if (peekedLine.type != INCR) {
        return SquareBracketsGroup{};
    }

    std::vector<Term> terms;
    do {
        terms.push_back(buildTerm());
        peekedLine = peekLine(tis);
        if (peekedLine.type == INCR) {
            SHOULD_NOT_HAPPEN(); // shouldnt happen after a call to buildTerm()
        }
    }
    until (peekedLine.nestingLevel <= parentNestingLevel || peekedLine.type == END);

    return SquareBracketsGroup{terms};
}

CurlyBracketsGroup LV1AstBuilder::buildCurlyBracketsGroup() {
    ENTERING_BUILD_ROUTINE();

    consumeLine(tis); // -> ... CurlyBracketsGroup

    auto peekedLine = peekLine(tis);
    if (peekedLine.type != INCR) {
        return CurlyBracketsGroup{};
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

    return CurlyBracketsGroup{sentences};
}

Association LV1AstBuilder::buildAssociation() {
    ENTERING_BUILD_ROUTINE();

    consumeLine(tis); // -> ... Association

    auto peekedLine = peekLine(tis);
    if (peekedLine.type != INCR) {
        SHOULD_NOT_HAPPEN(); // empty assoc
    }

    AssociationLeftPart leftPart = std::visit(overload{
        [](Association*) -> AssociationLeftPart {SHOULD_NOT_HAPPEN();},
        [](auto word) -> AssociationLeftPart {return word;}
    }, buildWord());

    peekedLine = peekLine(tis);
    if (peekedLine.type == INCR) {
        SHOULD_NOT_HAPPEN(); // shouldnt happen after a call to buildWord()
    }
    if (peekedLine.type == END) {
        SHOULD_NOT_HAPPEN(); // missing assoc right part
    }

    Word rightPart = buildWord();

    peekedLine = peekLine(tis);
    unless (peekedLine.type == DECR || peekedLine.type == END) {
        SHOULD_NOT_HAPPEN();
    }

    return Association{leftPart, rightPart};
}

PostfixParenthesesGroup LV1AstBuilder::buildPostfixParenthesesGroup() {
    ENTERING_BUILD_ROUTINE();

    consumeLine(tis); // -> ... PostfixParenthesesGroup

    auto peekedLine = peekLine(tis);
    if (peekedLine.type != INCR) {
        SHOULD_NOT_HAPPEN(); // empty ppg
    }

    auto word = buildWord();
    auto leftPart = std::visit(overload{
        [](Association*) -> AssociationLeftPart {SHOULD_NOT_HAPPEN();},
        [](auto* word) -> AssociationLeftPart {return word;}
    }, word);

    peekedLine = peekLine(tis);
    if (peekedLine.type == INCR) {
        SHOULD_NOT_HAPPEN(); // shouldnt happen after a call to buildWord()
    }
    if (peekedLine.type == END) {
        SHOULD_NOT_HAPPEN(); // missing ppg right part
    }

    ParenthesesGroup rightPart = buildParenthesesGroup();

    peekedLine = peekLine(tis);
    unless (peekedLine.type == DECR || peekedLine.type == END) {
        SHOULD_NOT_HAPPEN();
    }

    return PostfixParenthesesGroup{leftPart, rightPart};
}

Path LV1AstBuilder::buildPath() {
    ENTERING_BUILD_ROUTINE();

    consumeLine(tis); // -> ... Path

    auto peekedLine = peekLine(tis);
    if (peekedLine.type != INCR) {
        SHOULD_NOT_HAPPEN(); // empty path
    }

    auto word = buildWord();
    auto leftPart = std::visit(overload{
        [](Association*) -> AssociationLeftPart {SHOULD_NOT_HAPPEN();},
        [](auto* word) -> AssociationLeftPart {return word;}
    }, word);

    peekedLine = peekLine(tis);
    if (peekedLine.type == INCR) {
        SHOULD_NOT_HAPPEN(); // shouldnt happen after a call to buildWord()
    }
    if (peekedLine.type == END) {
        SHOULD_NOT_HAPPEN(); // missing path right part
    }

    Atom rightPart = buildAtom();

    peekedLine = peekLine(tis);
    unless (peekedLine.type == DECR || peekedLine.type == END) {
        SHOULD_NOT_HAPPEN();
    }

    return Path{leftPart, rightPart};
}

SquareBracketsTerm LV1AstBuilder::buildSquareBracketsTerm() {
    ENTERING_BUILD_ROUTINE();

    consumeLine(tis); // -> ... SquareBracketsTerm

    unless (peekLine(tis).type == INCR) {
        SHOULD_NOT_HAPPEN(); // empty sbt
    }
    auto term = buildTerm();

    auto peekedLine = peekLine(tis);
    unless (peekedLine.type == DECR || peekedLine.type == END) {
        SHOULD_NOT_HAPPEN(); // should only have 1 term
    }

    return SquareBracketsTerm{term};
}

PostfixSquareBracketsGroup LV1AstBuilder::buildPostfixSquareBracketsGroup() {
    ENTERING_BUILD_ROUTINE();

    consumeLine(tis); // -> ... PostfixSquareBracketsGroup

    auto peekedLine = peekLine(tis);
    if (peekedLine.type != INCR) {
        SHOULD_NOT_HAPPEN(); // empty ppg
    }

    auto word = buildWord();
    auto leftPart = std::visit(overload{
        [](Association*) -> AssociationLeftPart {SHOULD_NOT_HAPPEN();},
        [](auto* word) -> AssociationLeftPart {return word;}
    }, word);

    peekedLine = peekLine(tis);
    if (peekedLine.type == INCR) {
        SHOULD_NOT_HAPPEN(); // shouldnt happen after a call to buildWord()
    }
    if (peekedLine.type == END) {
        SHOULD_NOT_HAPPEN(); // missing ppg right part
    }

    SquareBracketsGroup rightPart = buildSquareBracketsGroup();

    peekedLine = peekLine(tis);
    unless (peekedLine.type == DECR || peekedLine.type == END) {
        SHOULD_NOT_HAPPEN();
    }

    return PostfixSquareBracketsGroup{leftPart, rightPart};
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
