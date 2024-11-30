#include <monlang-LV2/Statement.h>

/* impl only */
#include <monlang-LV2/stmt/Assignment.h>
#include <monlang-LV2/stmt/Accumulation.h>
#include <monlang-LV2/stmt/LetStatement.h>
#include <monlang-LV2/stmt/VarStatement.h>
#include <monlang-LV2/stmt/ExpressionStatement.h>

#include <utils/assert-utils.h>
#include <utils/mem-utils.h>

static ProgramSentence consumeSentence(LV1::Program& prog) {
    ASSERT (prog.sentences.size() > 0);
    auto res = prog.sentences[0];
    prog.sentences = std::vector(
        prog.sentences.begin() + 1,
        prog.sentences.end()
    );
    return res;
}

Statement consumeStatement(LV1::Program& prog, const context_t& cx) {
    auto& sentence = cx.sentence;
    ASSERT (prog.sentences.size() > 0);

    sentence = consumeSentence(prog);

    // if (sentence =~ "Word Atom<`:=`> Word+"_) {
    //     return move_to_heap(buildAssignment(sentence, cx));
    // }

    if (peekAssignment(sentence)) {
        return move_to_heap(buildAssignment(sentence, cx));
    }

    // if (sentence =~ "Word Atom<OPERATOR`=`> Word+"_) {
    //     return move_to_heap(buildAccumulation(sentence, cx));
    // }

    if (peekAccumulation(sentence)) {
        return move_to_heap(buildAccumulation(sentence, cx));
    }

    if (peekLetStatement(sentence)) {
        return move_to_heap(buildLetStatement(sentence, cx));
    }

    if (peekVarStatement(sentence)) {
        return move_to_heap(buildVarStatement(sentence, cx));
    }

    // ...

    /* fall-through statement */
    return move_to_heap(buildExpressionStatement(sentence, cx));
}
