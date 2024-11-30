#include <monlang-LV2/Statement.h>

/* impl only */
#include <monlang-LV2/stmt/Assignment.h>
#include <monlang-LV2/stmt/Accumulation.h>
#include <monlang-LV2/stmt/LetStatement.h>
#include <monlang-LV2/stmt/VarStatement.h>
#include <monlang-LV2/stmt/Guard.h>
#include <monlang-LV2/stmt/ReturnStatement.h>
#include <monlang-LV2/stmt/BreakStatement.h>
#include <monlang-LV2/stmt/ContinueStatement.h>
#include <monlang-LV2/stmt/DieStatement.h>
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

    // if (sentence =~ "ProgramWord Atom<`:=`> ProgramWord+"_) {
    //     return move_to_heap(buildAssignment(sentence, cx));
    // }

    if (peekAssignment(sentence)) {
        return move_to_heap(buildAssignment(sentence, cx));
    }

    // if (sentence =~ "ProgramWord Atom<OPERATOR`=`> ProgramWord+"_) {
    //     return move_to_heap(buildAccumulation(sentence, cx));
    // }

    if (peekAccumulation(sentence)) {
        return move_to_heap(buildAccumulation(sentence, cx));
    }

    // if (sentence =~ "Atom<`let`> ProgramWord ProgramWord+"_) {
    //     return move_to_heap(buildLetStatement(sentence, cx));
    // }

    if (peekLetStatement(sentence)) {
        return move_to_heap(buildLetStatement(sentence, cx));
    }

    // if (sentence =~ "Atom<`var`> ProgramWord ProgramWord+"_) {
    //     return move_to_heap(buildVarStatement(sentence, cx));
    // }

    if (peekVarStatement(sentence)) {
        return move_to_heap(buildVarStatement(sentence, cx));
    }

    // if (sentence =~ "Atom<`return`> ProgramWord*"_) {
    //     return move_to_heap(buildReturnStatement(sentence, cx));
    // }

    if (peekReturnStatement(sentence)) {
        return move_to_heap(buildReturnStatement(sentence, cx));
    }

    // if (sentence =~ "Atom<`break`> ProgramWord*"_) {
    //     return move_to_heap(BreakStatement{});
    // }

    if (peekBreakStatement(sentence)) {
        return move_to_heap(BreakStatement{});
    }

    // if (sentence =~ "Atom<`continue`> ProgramWord*"_) {
    //     return move_to_heap(ContinueStatement{});
    // }

    if (peekContinueStatement(sentence)) {
        return move_to_heap(ContinueStatement{});
    }

    // if (sentence =~ "Atom<`die`> ProgramWord*"_) {
    //     return move_to_heap(DieStatement{});
    // }

    if (peekDieStatement(sentence)) {
        return move_to_heap(DieStatement{});
    }

    // if (peekGuard(sentence)) {
    //     return move_to_heap(buildGuard(sentence, cx));
    // }

    // ...

    /* fall-through statement */
    return move_to_heap(buildExpressionStatement(sentence, cx));
}
