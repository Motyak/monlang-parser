#include <monlang-LV2/Program.h>

#include <utils/loop-utils.h>
#include <utils/assert-utils.h>

LV2::Program consumeProgram(LV1::Program& prog, context_t* cx) {
    auto& statements = *std::any_cast<std::vector<Statement>*>(cx->statements);
    auto& malformed_stmt = *cx->malformed_stmt;
    auto& fallthrough = *cx->fallthrough;

    until (prog.sentences.empty()) {
        auto statement = consumeStatement(prog, cx);
        if (malformed_stmt || fallthrough) {
            return LV2::Program(); // stub
        }
        statements.push_back(statement);
    }

    return LV2::Program{statements};
}
