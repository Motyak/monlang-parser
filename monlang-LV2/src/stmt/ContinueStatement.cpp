#include <monlang-LV2/stmt/ContinueStatement.h>

/* impl only */
#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/ProgramSentence.h>

#define unless(x) if(!(x))

static Atom AtomConstant(const std::string& val) {
    auto atom = Atom{val};
    atom._tokenLen = val.size();
    return atom;
}

const Atom ContinueStatement::KEYWORD = AtomConstant("continue");

bool peekContinueStatement(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() >= 1) {
        return false;
    }

    auto pw = sentence.programWords[0];
    unless (std::holds_alternative<Atom*>(pw)) {
        return false;
    }

    auto atom = *std::get<Atom*>(pw);
    return atom.value == ContinueStatement::KEYWORD.value;
}
