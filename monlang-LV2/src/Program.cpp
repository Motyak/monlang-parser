#include <monlang-LV2/Program.h>

#include <utils/loop-utils.h>
#include <utils/assert-utils.h>
#include <utils/vec-utils.h>

MayFail<MayFail_<LV2::Program>> consumeProgram(LV1::Program& prog) {
    std::vector<MayFail<Statement_>> statements;
    MayFail<Statement_> currentStatement;

    until (prog.sentences.empty()) {
        currentStatement = consumeStatement(prog);
        statements.push_back(currentStatement);
        if (currentStatement.has_error()) {
            return Malformed(MayFail_<LV2::Program>{statements}, ERR(111));
        }
    }

    return MayFail_<LV2::Program>{statements};
}

LV2::Program::Program(const std::vector<Statement>& statements) : statements(statements){}

MayFail_<LV2::Program>::MayFail_(const std::vector<MayFail<Statement_>>& statements) : statements(statements){}

MayFail_<LV2::Program>::MayFail_(LV2::Program prog) {
    std::vector<MayFail<Statement_>> res;
    for (auto e: prog.statements) {
        res.push_back(wrap_stmt(e));
    }
    this->statements = res;
    this->_tokenId = prog._tokenId;
}

MayFail_<LV2::Program>::operator LV2::Program() const {
    std::vector<Statement> statements;
    for (auto e: this->statements) {
        statements.push_back(unwrap_stmt(e.value()));
    }
    auto prog = LV2::Program{statements};
    prog._tokenId = this->_tokenId;
    return prog;
}
