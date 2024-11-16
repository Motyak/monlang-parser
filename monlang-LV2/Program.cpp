#include <monlang-LV2/Program.h>

#include <utils/loop-utils.h>

LV2::Program consumeProgram(LV1::Program& prog, context_t cx) {
    ASSERT (cx.statements.empty());
    until (prog.sentences.empty()) {
        cx.statements.push_back(consumeStatement(prog, cx));
    }
    return LV2::Program{cx.statements};
}
