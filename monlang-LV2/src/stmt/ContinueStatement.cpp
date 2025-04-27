#include <monlang-LV2/stmt/ContinueStatement.h>

/* impl only */
#include <monlang-LV1/ast/Atom.h>

#include <utils/assert-utils.h>
#include <utils/mem-utils.h>

#define unless(x) if(!(x))

const std::string ContinueStatement::KEYWORD = "continue";

bool peekContinueStatement(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 1) {
        return false;
    }

    auto pw = sentence.programWords[0];
    unless (std::holds_alternative<Atom*>(pw)) {
        return false;
    }

    auto atom = *std::get<Atom*>(pw);
    return atom.value == ContinueStatement::KEYWORD;
}

static ProgramSentence consumeSentence(LV1::Program&);

ContinueStatement* consumeContinueStatement(LV1::Program& prog) {
    auto sentence = consumeSentence(prog);
    auto continueStmt = ContinueStatement{};
    continueStmt._tokenLeadingNewlines = sentence._tokenLeadingNewlines;
    continueStmt._tokenIndentSpaces = sentence._tokenIndentSpaces;
    continueStmt._tokenLen = sentence._tokenLen;
    continueStmt._tokenTrailingNewlines = sentence._tokenTrailingNewlines;
    return move_to_heap(continueStmt);
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
