#include <monlang-LV2/stmt/BreakStatement.h>

/* impl only */
#include <monlang-LV1/ast/Atom.h>

#include <utils/assert-utils.h>
#include <utils/mem-utils.h>

#define unless(x) if(!(x))

static Atom AtomConstant(const std::string& val) {
    auto atom = Atom{val};
    atom._tokenLen = val.size();
    return atom;
}

const Atom BreakStatement::KEYWORD = AtomConstant("break");

bool peekBreakStatement(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 1) {
        return false;
    }

    auto pw = sentence.programWords[0];
    unless (std::holds_alternative<Atom*>(pw)) {
        return false;
    }

    auto atom = *std::get<Atom*>(pw);
    return atom.value == BreakStatement::KEYWORD.value;
}

static ProgramSentence consumeSentence(LV1::Program&);

BreakStatement* consumeBreakStatement(LV1::Program& prog) {
    auto sentence = consumeSentence(prog);
    auto breakStmt = BreakStatement{};
    breakStmt._tokenLeadingNewlines = sentence._tokenLeadingNewlines;
    breakStmt._tokenIndentSpaces = sentence._tokenIndentSpaces;
    breakStmt._tokenLen = sentence._tokenLen;
    breakStmt._tokenTrailingNewlines = sentence._tokenTrailingNewlines;
    return move_to_heap(breakStmt);
}

static ProgramSentence consumeSentence(LV1::Program& prog) {
    ASSERT (prog.sentences.size() > 0);
    auto res = prog.sentences[0];
    prog.sentences = std::vector(
        prog.sentences.begin() + 1,
        prog.sentences.end()
    );
    return res;
}
