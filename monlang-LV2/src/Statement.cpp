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
#include <monlang-LV2/stmt/ForeachStatement.h>
#include <monlang-LV2/stmt/WhileStatement.h>
#include <monlang-LV2/stmt/ExpressionStatement.h>

#include <utils/assert-utils.h>
#include <utils/mem-utils.h>
#include <utils/variant-utils.h>

#define unless(x) if(!(x))

static ProgramSentence consumeSentence(LV1::Program&);

MayFail<Statement_> consumeStatement(LV1::Program& prog) {
    ASSERT (prog.sentences.size() > 0);
    auto peekedSentence = prog.sentences[0];

    // if (peekedSentence =~ "ProgramWord Atom<`:=`> ProgramWord*"_) {
    //     return mayfail_convert<Statement_>(consumeAssignment(prog));
    // }

    if (peekAssignment(peekedSentence)) {
        return mayfail_convert<Statement_>(consumeAssignment(prog));
    }

    // if (peekedSentence =~ "ProgramWord Atom<OPERATOR`=`> ProgramWord*"_) {
    //     return mayfail_convert<Statement_>(consumeAccumulation(prog));
    // }

    if (peekAccumulation(peekedSentence)) {
        return mayfail_convert<Statement_>(consumeAccumulation(prog));
    }

    // if (peekedSentence =~ "Atom<`let`> ProgramWord*"_) {
    //     return mayfail_convert<Statement_>(consumeLetStatement(prog));
    // }

    if (peekLetStatement(peekedSentence)) {
        return mayfail_convert<Statement_>(consumeLetStatement(prog));
    }

    // if (peekedSentence =~ "Atom<`var`> ProgramWord*"_) {
    //     return mayfail_convert<Statement_>(consumeVarStatement(prog));
    // }

    if (peekVarStatement(peekedSentence)) {
        return mayfail_convert<Statement_>(consumeVarStatement(prog));
    }

    // if (peekedSentence =~ "Atom<`return`> ProgramWord*"_) {
    //     return mayfail_convert<Statement_>(consumeReturnStatement(prog));
    // }

    if (peekReturnStatement(peekedSentence)) {
        return mayfail_convert<Statement_>(consumeReturnStatement(prog));
    }

    // if (peekedSentence =~ "Atom<`break`> ProgramWord*"_) {
    //     return (Statement_)move_to_heap(BreakStatement{});
    // }

    if (peekBreakStatement(peekedSentence)) {
        return (Statement_)consumeBreakStatement(prog);
    }

    // if (peekedSentence =~ "Atom<`continue`> ProgramWord*"_) {
    //     return (Statement_)move_to_heap(ContinueStatement{});
    // }

    if (peekContinueStatement(peekedSentence)) {
        return (Statement_)consumeContinueStatement(prog);
    }

    // if (peekedSentence =~ "Atom<`die`> ProgramWord*"_) {
    //     return (Statement_)move_to_heap(DieStatement{});
    // }

    if (peekDieStatement(peekedSentence)) {
        return (Statement_)consumeDieStatement(prog);
    }

    // if (peekedSentence =~ "Atom<`foreach`> ProgramWord*"_) {
    //     return mayfail_convert<Statement_>(consumeForeachStatement(prog));
    // }

    if (peekForeachStatement(peekedSentence)) {
        return mayfail_convert<Statement_>(consumeForeachStatement(prog));
    }

    // if (peekedSentence =~ "Atom<while|until> ProgramWord*"_) {
    //     return mayfail_convert<Statement_>(consumeForeachStatement(prog));
    // }

    if (peekWhileStatement(peekedSentence)) {
        return mayfail_convert<Statement_>(consumeWhileStatement(prog));
    }

    // if (peekedSentence =~ "Atom<`do`> ProgramWord*"_) {
    //     return mayfail_convert<Statement_>(consumeForeachStatement(prog));
    // }

    if (peekDoWhileStatement(peekedSentence)) {
        return mayfail_convert<Statement_>(consumeDoWhileStatement(prog));
    }

    // if (peekGuard(peekedSentence)) {
    //     return mayfail_convert<Statement_>(consumeGuard(prog));
    // }

    // ...

    for (auto pw: peekedSentence.programWords) {
        unless (holds_word(pw)) {
            consumeSentence(prog);
            return Malformed(Statement_(), ERR(121));
        }
    }

    /* fall-through statement */
    return mayfail_convert<Statement_>(consumeExpressionStatement(prog));
}

static ProgramSentence consumeSentence(LV1::Program& prog) {
    ASSERT (prog.sentences.size() > 0);
    auto res = prog.sentences[0];
    prog.sentences = std::vector(
        prog.sentences.begin() + 1,
        prog.sentences.end()
    );
    return res;
}

Statement unwrap_stmt(Statement_ statement) {
    return std::visit(overload{
        [](BreakStatement* stmt) -> Statement {return stmt;},
        [](ContinueStatement* stmt) -> Statement {return stmt;},
        [](DieStatement* stmt) -> Statement {return stmt;},
        [](auto* mf_) -> Statement {return move_to_heap(unwrap(*mf_));},
    }, statement);
}

Statement_ wrap_stmt(Statement statement) {
    return std::visit(overload{
        [](BreakStatement* stmt) -> Statement_ {return stmt;},
        [](ContinueStatement* stmt) -> Statement_ {return stmt;},
        [](DieStatement* stmt) -> Statement_ {return stmt;},
        [](auto* stmt) -> Statement_ {return move_to_heap(wrap(*stmt));},
    }, statement);
}
