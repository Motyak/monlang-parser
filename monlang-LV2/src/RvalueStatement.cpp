#include <monlang-LV2/RvalueStatement.h>
#include <monlang-LV2/common.h>

#include <utils/assert-utils.h>

RvalueStatement buildRvalueStatement(const ProgramSentence& sentence, const context_t& cx) {
    auto& term = cx.term;
    ASSERT (sentence.programWords.size() > 0);
    ASSERT (term.words.empty()); //

    term = toTerm(sentence);
    return RvalueStatement{buildRvalue(term, cx)};
}
