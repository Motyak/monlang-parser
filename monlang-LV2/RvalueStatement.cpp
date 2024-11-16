#include <monlang-LV2/RvalueStatement.h>
#include <monlang-LV2/common.h>

#include <utils/assert-utils.h>

RvalueStatement buildRvalueStatement(const ProgramSentence& sentence) {
    ASSERT (sentence.programWords.size() > 0);
    auto term = toTerm(sentence);
    return RvalueStatement{buildRvalue(term)};
}
