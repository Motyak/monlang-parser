#include <monlang-LV2/Program.h>

#include <utils/loop-utils.h>

LV2::Program consumeProgram(LV1::Program& prog) {
    std::vector<Statement> statements;
    until (prog.sentences.empty()) {
        statements.push_back(consumeStatement(prog));
    }
    return LV2::Program{statements};
}
