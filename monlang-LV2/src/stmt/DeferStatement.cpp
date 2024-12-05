#include <monlang-LV2/stmt/DeferStatement.h>

/* impl only */
#include <monlang-LV1/ast/Atom.h>

#include <utils/assert-utils.h>

#define unless(x) if(!(x))

bool peekDeferStatement(const ProgramSentence& sentence) {
    unless (sentence.programWords.size() == 2) {
        return false;
    }
    unless (std::holds_alternative<Atom*>(sentence.programWords[0])) {
        return false;
    }
    auto atom = *std::get<Atom*>(sentence.programWords[0]);
    return atom.value == "defer";
}

DeferStatement buildDeferStatement(const ProgramSentence& sentence, context_t* cx) {
    TODO();
}
