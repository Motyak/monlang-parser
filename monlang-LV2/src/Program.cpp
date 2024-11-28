#include <monlang-LV2/Program.h>

#include <utils/loop-utils.h>
#include <utils/assert-utils.h>

LV2::Program consumeProgram(LV1::Program& prog, const context_t& cx) {
    auto& statements = *std::any_cast<std::vector<Statement>*>(cx.statements);

    until (prog.sentences.empty()) {
        auto statement = consumeStatement(prog, cx);
        if (cx.malformed_stmt || cx.fallthrough) {
            return LV2::Program(); // stub
        }
        statements.push_back(statement);
    }

    return LV2::Program{statements};
}
