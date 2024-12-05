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

MayFail_<LV2::Program>::MayFail_(std::vector<MayFail<Statement_>> statements) : statements(statements){}

MayFail_<LV2::Program>::MayFail_(LV2::Program prog) : MayFail_(wrap(prog)){}

MayFail_<LV2::Program>::operator LV2::Program() const {
    std::vector<Statement> res;
    for (auto e: statements) {
        res.push_back(unwrap_stmt(e.value()));
    }
    return LV2::Program{res};
}

template <>
LV2::Program unwrap(const MayFail_<LV2::Program>& prog) {
    return (LV2::Program)prog;
}

template <>
MayFail_<LV2::Program> wrap(const LV2::Program& prog) {
    std::vector<MayFail<Statement_>> res;
    for (auto e: prog.statements) {
        res.push_back(wrap_stmt(e));
    }
    return MayFail_<LV2::Program>{res};
}
