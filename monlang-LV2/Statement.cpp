#include <monlang-LV2/Statement.h>
#include <monlang-LV2/RvalueStatement.h>
#include <monlang-LV2/common.h>

#include <utils/assert-utils.h>

Statement consumeStatement(LV1::Program& prog) {
    ASSERT (prog.sentences.size() > 0);
    auto peekedSentence = prog.sentences[0];

    // if (peekedSentence =~ "LVALUE Atom<`:=`> RVALUE"_) {
    //     return consumeAssignment(prog)
    // }

    // ...

    // fall-through statement
    return move_to_heap(buildRvalueStatement(consumeSentence(prog)));
}