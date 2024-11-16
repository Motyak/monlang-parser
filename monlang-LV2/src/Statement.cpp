#include <monlang-LV2/Statement.h>
#include <monlang-LV2/RvalueStatement.h>
#include <monlang-LV2/common.h>

#include <utils/assert-utils.h>

Statement consumeStatement(LV1::Program& prog, const context_t& cx) {
    auto& sentence = cx.sentence;
    ASSERT (prog.sentences.size() > 0);
    ASSERT (sentence.programWords.empty()); //

    auto peekedSentence = prog.sentences[0];

    // if (peekedSentence =~ "LVALUE Atom<`:=`> RVALUE"_) {
    //     return consumeAssignment(prog)
    // }

    // ...

    /* fall-through statement */
    sentence = consumeSentence(prog);
    return move_to_heap(buildRvalueStatement(sentence, cx));
}