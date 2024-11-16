#include <monlang-LV2/RvalueStatement.h>
#include <monlang-LV2/common.h>

#include <utils/assert-utils.h>

RvalueStatement buildRvalueStatement(const ProgramSentence& sentence, context_t cx) {
    ASSERT (sentence.programWords.size() > 0);
    cx.term = toTerm(sentence);
    return RvalueStatement{buildRvalue(cx.term, cx)};
}
