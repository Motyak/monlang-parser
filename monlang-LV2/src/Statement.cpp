#include <monlang-LV2/Statement.h>

/* impl only */
#include <monlang-LV2/ExpressionStatement.h>
#include <monlang-LV2/common.h>

#include <utils/assert-utils.h>
#include <utils/mem-utils.h>

Statement consumeStatement(LV1::Program& prog, const context_t& cx) {
    auto& sentence = cx.sentence;
    ASSERT (prog.sentences.size() > 0);

    auto peekedSentence = prog.sentences[0];

    // if (peekedSentence =~ "LVALUE Atom<`:=`> EXPRESSION"_) {
    //     return consumeAssignment(prog)
    // }

    // ...

    /* fall-through statement */
    sentence = consumeSentence(prog);
    return move_to_heap(buildExpressionStatement(sentence, cx));
}
