#include <monlang-LV2/Program.h>

#include <utils/loop-utils.h>

LV2::Program consumeProgram(LV1::Program& prog, context_t cx) {
    auto* statements = std::any_cast<std::vector<Statement>*>(cx.statements);
    ASSERT (statements != nullptr && statements->empty());

    until (prog.sentences.empty()) {
        statements->push_back(consumeStatement(prog, cx));
    }
    return LV2::Program{*statements};
}
